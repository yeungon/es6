/* 
https://github.com/libuv/libuv/blob/v1.x/src/unix/core.c
https://blog.insiderattack.net/handling-io-nodejs-event-loop-part-4-418062f917d1
https://www.youtube.com/watch?v=sGTRmPiXD4Y
 */

int uv_run(uv_loop_t *loop, uv_run_mode mode)
{
  int timeout;
  int r;
  int ran_pending;

  r = uv__loop_alive(loop);
  // Kiểm tra có tác vụ nào không, nếu true sẽ tiếp tục chạy, xem ở dưới

  if (!r)
    uv__update_time(loop); // Cập nhật loop time hiện tại

  while (r != 0 && loop->stop_flag == 0)
  {
    uv__update_time(loop);               //Phrase 1: Cập nhật loop time hiện tại
    uv__run_timers(loop);                //Phrase 2: Chaỵ các callback Timer "tới kì"
    ran_pending = uv__run_pending(loop); //Phrase 3: Chạy các callback I/O hoàn thành, nếu không có thì trả về 0, nếu có thì chạy và trả về 1
    uv__run_idle(loop);                  //Phrase 4: libuv internal stuff.
    uv__run_prepare(loop);               //Phrase 5: Chuẩn bị

    timeout = 0;
    if ((mode == UV_RUN_ONCE && !ran_pending) || mode == UV_RUN_DEFAULT)
      timeout = uv_backend_timeout(loop); //timeout dùng để tính sẽ block cho I/O ở phrase 5, nếu bằng 0 --> bước I/O sẽ skip
    uv__io_poll(loop, timeout);    //Phrase 6: Poll for I/O
    uv__run_check(loop);           //Phrase 7: (setImmediate  --> executed after I/O operations during its event loop.
    uv__run_closing_handles(loop); //Phrase 8: Run all close handlers

    if (mode == UV_RUN_ONCE)
    {
      /* UV_RUN_ONCE implies forward progress: at least one callback must have
       * been invoked when it returns. uv__io_poll() can return without doing
       * I/O (meaning: no callbacks) when its timeout expires - which means we
       * have pending timers that satisfy the forward progress constraint.
       *
       * UV_RUN_NOWAIT makes no guarantees about progress so it's omitted from
       * the check.
       */
      uv__update_time(loop);
      uv__run_timers(loop);
    }

    r = uv__loop_alive(loop);
    if (mode == UV_RUN_ONCE || mode == UV_RUN_NOWAIT)
      break;
  }

  if (loop->stop_flag != 0)
    loop->stop_flag = 0;

  return r;
}

// Có sự kiện nào cần loop
static int uv__loop_alive(const uv_loop_t* loop) {
  return uv__has_active_handles(loop) ||
         uv__has_active_reqs(loop) ||
         loop->closing_handles != NULL;
}


//Dựa vào timeout để xác định liệu có block hay không để đợi I/O mới, nếu khác 0 mới block.
int uv_backend_timeout(const uv_loop_t* loop) {
  if (loop->stop_flag != 0)
    return 0;
  //block 0 giây nếu không có tác vụ nào "sống", đây chính là hàm uv__loop_alive ở trên--> thoát loop
  if (!uv__has_active_handles(loop) && !uv__has_active_reqs(loop))
    return 0;
//block 0 giây nếu có tác vụ ĐANG xử lý --> thoát loop
  if (!QUEUE_EMPTY(&loop->idle_handles))
    return 0;
//block 0 giây nếu có tác ĐANG xử lý --> thoát loop
  if (!QUEUE_EMPTY(&loop->pending_queue))
    return 0;
//block 0 giây để chuyển sang close phase
  if (loop->closing_handles)
    return 0;

  return uv__next_timeout(loop);
}

//Tính thời gian sẽ block đề chờ

/* until this timeout expires or system-specified maximum safe timeout reaches. After the timeout, event loop will again become active and move on to the “check handlers” phase */

int uv__next_timeout(const uv_loop_t* loop) {
  const struct heap_node* heap_node;
  const uv_timer_t* handle;
  uint64_t diff;

  heap_node = heap_min((const struct heap*) &loop->timer_heap);
  if (heap_node == NULL)
    return -1; /* block indefinitely */

  handle = container_of(heap_node, uv_timer_t, heap_node);
  if (handle->timeout <= loop->time)
    return 0;

  diff = handle->timeout - loop->time;
  if (diff > INT_MAX)
    diff = INT_MAX;

  return diff;
}

/* Hàm uv_backend_timeout
If the loop’s stop_flag is set which determines the loop is about to exit, timeout is 0.
If there are no active handles or active operations pending, there’s no point of waiting, therefore the timeout is 0.
If there are pending idle handles to be executed, waiting for I/O should not be done. Therefore, the timeout is 0.
If there are completed I/O handlers in pending_queue, waiting for I/O should not be done. Therefore the timeout is 0.
If there are any close handlers pending to be executed, should not wait for I/O. Therefore, the timeout is 0. 

If none of the above criteria is met, uv__next_timeout method is called to determine how long libuv should wait for I/O.
What uv__next_timeout does is, it will return the value of the closest timer’s value. And if there are no timers, it will return -1 indicating infinity.

*/


/* UV_RUN_DEFAULT: Runs the event loop until there are no more active and referenced handles or requests. Returns non-zero if uv_stop() was called and there are still active handles or requests. Returns zero in all other cases.
UV_RUN_ONCE: Poll for i/o once. Note that this function blocks if there are no pending callbacks. Returns zero when done (no active handles or requests left), or non-zero if more callbacks are expected (meaning you should run the event loop again sometime in the future).
UV_RUN_NOWAIT: Poll for i/o once but don’t block if there are no pending callbacks. Returns zero if done (no active handles or requests left), or non-zero if more callbacks are expected (meaning you should run the event loop again sometime in the future). 

http://docs.libuv.org/en/v1.x/loop.html
*/