/* This file is located at: https://github.com/yeungon/javascript*/
/* 
References:
https://github.com/libuv/libuv/blob/v1.x/src/unix/core.c
https://static.googleusercontent.com/media/research.google.com/en//pubs/archive/46255.pdf (Semantics of Asynchronous JavaScript)
https://blog.insiderattack.net/handling-io-nodejs-event-loop-part-4-418062f917d1
https://github.com/nodejs/help/issues/1118 (Hỏi đáp về event loop, khá okey)
https://jakearchibald.com/2015/tasks-microtasks-queues-and-schedules/ (event loop on browser only)
https://www.youtube.com/watch?v=cCOL7MC4Pl0 by Jake Archibald (browser only)
https://www.youtube.com/watch?v=VfYmKD-SvjU by Colin Ihrig (ntroduction to libuv: What's a Unicorn Velociraptor?)
https://www.youtube.com/watch?v=8aGhZQkoFbQ by Philip Roberts -> the best video about asynchronous (only browser)
https://www.youtube.com/watch?v=sGTRmPiXD4Y by Saul Ibarra Coretge --> libuv explained in C code, good one.
https://www.youtube.com/watch?v=P9csgxBgaZ8 by Sam Roberts
https://www.youtube.com/watch?v=gl9qHml-mKc by Danie Khan
https://www.youtube.com/watch?v=u1kqx6AenYw by Erin Zimmer (Chú ý thứ tự nextTick và Promise chưa đúng) 
https://www.youtube.com/watch?v=PNa9OMajw9w by Bert Belder (Chú ý thứ tự nextTick và Promise chưa đúng)
https://www.youtube.com/watch?v=9y67eMUzj9A (also mention THREADPOOL)
https://www.youtube.com/watch?v=7f787SsgknA (Сергей Аванесян, spoken in Russian)
https://www.facebook.com/groups/jsland/permalink/427721001124549/ (by Chau Tran - THREADPOOL, note: the size of threadpool is (libuv bản 1.30.0) now 1024)
https://frontendmasters.com/courses/servers-node-js/timer-queue/ (paid course) --> good to understand asynchronous (no mention THREADPOOL)
https://www.udemy.com/course/advanced-node-for-developers/ (paid course)    --> good to understand asynchronous (also mention THREADPOOL)
https://www.udemy.com/course/nodejs-express-mongodb-bootcamp/ (paid course) --> good to understand asynchronous (also mention THREADPOOL)
http://nikhilm.github.io/uvbook/basics.html#event-loops
https://dev.to/lydiahallie/javascript-visualized-event-loop-3dif (Visulisation of event loop - browser only)
https://github.com/nodejs/node/pull/22842 (Thay đổi về MicroTask trên Node11, để giống browser)
https://github.com/nodejs/nodejs.org/pull/1804 (Trao đổi thêm về MicroTask trên Node11, để giống browser)
https://stackoverflow.com/questions/47724811/why-setimmediate-execute-before-fs-readfile-in-nodejs-event-loops-works
https://gist.github.com/thlorenz/8dd5d3a50ee14457afce#uv__run_timers
http://voidcanvas.com/setimmediate-vs-nexttick-vs-settimeout/ (quite good article)
https://www.fatalerrors.org/a/nodejs-series-q-a-s-understanding-of-event-loop-timers-and-process.nexttick.html ==> 
"Here we only care about UV_RUN_DEFAULT, because Node event loop uses this mode."
https://stackoverflow.com/questions/26740888/how-node-js-event-loop-model-scales-well ==> the diagram in this page is the best on the internet :-)
https://stackoverflow.com/questions/55467033/difference-between-process-nexttick-and-queuemicrotask : process.nextTick and queueMicrotask
http://voidcanvas.com/nodejs-event-loop/
 */

/* 
The following code is from: https://github.com/libuv/libuv/blob/v1.x/src/unix/core.c
 */
int uv_run(uv_loop_t *loop, uv_run_mode mode)
{
  int timeout;
  int r;
  int ran_pending;

  r = uv__loop_alive(loop);
  // Kiểm tra có tác vụ nào không, nếu true sẽ tiếp tục chạy, xem ở dưới

  if (!r)
    uv__update_time(loop); // Gọi tới uv__hrtime --> gọi tơi clock_gettime của LINUX. Nếu loop là inactive, tức uv__loop_alive trả về FALSE, Lấy THỜI GIAN loop time lần 1 ==> Update the event loop’s concept of “now”.

  while (r != 0 && loop->stop_flag == 0)
  {
    uv__update_time(loop);               //Lấy THỜI GIAN loop time: Khi có r là true
    uv__run_timers(loop);                //Phase 1: TIMER PHASE: Chaỵ các callback Timer "tới kì" (expired timers) --> Lần 1. Điều kiện loop time > (lớn hơn) timeout của setTimeout. Timer của setTimeout sẽ được sắp xếp theo thứ tự tăng dần, chỉ check tiếp nếu looptime > timeout của setTimeout
    ran_pending = uv__run_pending(loop); //Phase 2: PENDING CALLBACK: Chạy các callback I/O hoàn thành/lỗi chứa trong pending_queue của libuv đã hoàn thành, sau đó trả về 1. Nếu pending_queue rỗng, trả về 0;
    uv__run_idle(loop);                  //Phase 3: libuv internal stuff.
    uv__run_prepare(loop);               //Phase 4: Chuẩn bị

    timeout = 0;
    if ((mode == UV_RUN_ONCE && !ran_pending) || mode == UV_RUN_DEFAULT)
      //Here we only care about UV_RUN_DEFAULT, because Node event loop uses this mode.
      /*
      ::::---> UV_RUN_DEFAULT: Runs the event loop until there are no more active and referenced handles or requests. Returns non-zero if uv_stop() was called and there are still active handles or requests. Returns zero in all other cases.
      ::::---> UV_RUN_ONCE: Poll for i/o once. Note that this function blocks if there are no pending callbacks. Returns zero when done (no active handles or requests left), or non-zero if more callbacks are expected (meaning you should run the event loop again sometime in the future).
      ::::---> UV_RUN_NOWAIT: Poll for i/o once but don’t block if there are no pending callbacks. Returns zero if done (no active handles or requests left), or non-zero if more callbacks are expected (meaning you should run the event loop again sometime in the future). */

      timeout = uv_backend_timeout(loop); //timeout dùng để tính sẽ block cho I/O ở phrase 5 trong bao lâu, nếu bằng 0 --> bước I/O sẽ skip
    uv__io_poll(loop, timeout);           //Phase 5: POLL for I/O: it’s optional (), có block tùy thuộc vào timeout. Skip nếu timeout bằng 0.
    /* This function will watch for any incoming I/O operations until this timeout expires or system-specified maximum safe timeout reaches. After the timeout, event loop will again become active and move on to the “check handlers” phase.
    I/O Polling ==> performed by epoll_wait kernel system calls, on macOS using kqueue. In Windows, it’s performed using GetQueuedCompletionStatus in IOCP(Input Output Completion Port) */
    uv__run_check(loop);           //Phase 6: CHECK PHASE (setImmediate  --> executed after I/O operations during its event loop.
    uv__run_closing_handles(loop); //Phase 7: CLOSE Run all close handlers

    if (mode == UV_RUN_ONCE)
    {
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
static int uv__loop_alive(const uv_loop_t *loop)
{
  return uv__has_active_handles(loop) ||
         uv__has_active_reqs(loop) ||
         loop->closing_handles != NULL;
}

//Chạy PENDING CALLBACK: nếu có pending_queue, chạy và trả về 1, nếu không có trả về 0;
static int uv__run_pending(uv_loop_t *loop)
{
  QUEUE *q;
  QUEUE pq;
  uv__io_t *w;

  if (QUEUE_EMPTY(&loop->pending_queue))
    return 0;

  QUEUE_MOVE(&loop->pending_queue, &pq);

  while (!QUEUE_EMPTY(&pq))
  {
    q = QUEUE_HEAD(&pq);
    QUEUE_REMOVE(q);
    QUEUE_INIT(q);
    w = QUEUE_DATA(q, uv__io_t, pending_queue);
    w->cb(loop, w, POLLOUT);
  }

  return 1;
}

//Dựa vào timeout để xác định liệu có block hay không để đợi I/O mới, nếu khác 0 mới block.
int uv_backend_timeout(const uv_loop_t *loop)
{
  //If event loop has (or is) ended (calling uv_stop(), stop_flag!= 0), timeout is 0
  if (loop->stop_flag != 0)
    return 0;
  //block 0 giây nếu không có tác vụ asynchronous nào "sống", đây chính là hàm uv__loop_alive ở trên--> chạy tiếp
  if (!uv__has_active_handles(loop) && !uv__has_active_reqs(loop))
    return 0;
  //block 0 giây nếu có tác vụ ĐANG xử lý --> chạy tiếp
  if (!QUEUE_EMPTY(&loop->idle_handles))
    return 0;
  //block 0 giây nếu có tác vụ ĐANG xử lý --> chạy tiếp
  if (!QUEUE_EMPTY(&loop->pending_queue))
    return 0;
  //block 0 giây để chuyển sang close phase

  /*  If scripts have been scheduled by setImmediate(), "polling" phase will set a time-out which is zero.It means that after the queue has been exhausted, "polling" phase will not wait for callbacks to be added to the queue but continue to the check phase.

  If scripts have been scheduled by setTimeout(), "polling" will set a time-out which is the result of the soonest threshold of timers minus (trừ đi) current time.Then when time out, the loop continues and finally wraps back to the timers phase. 
  @https://stackoverflow.com/questions/56153710/why-settimeout-and-setiimidiate-are-deterministic-only-in-io-cycle

*/

  if (loop->closing_handles)
    return 0;
  //Tính thời gian sẽ block I/O đề chờ, nếu các điều kiện trên FALSE
  return uv__next_timeout(loop);
}

/* until this timeout expires or system-specified maximum safe timeout reaches. After the timeout, event loop will again become active and move on to the “check handlers” phase */

int uv__next_timeout(const uv_loop_t *loop)
{
  const struct heap_node *heap_node;
  const uv_timer_t *handle;
  uint64_t diff;

  heap_node = heap_min((const struct heap *)&loop->timer_heap);
  if (heap_node == NULL)
    return -1; /* block mãi mãi nếu không có timers*/

  handle = container_of(heap_node, uv_timer_t, heap_node);
  if (handle->timeout <= loop->time)
    return 0; // if next timer expired

  diff = handle->timeout - loop->time;
  if (diff > INT_MAX)
    diff = INT_MAX;

  return diff;
  /* returns time until next timer expires ==>
  What uv__next_timeout does is, it will return the value of the closest timer’s value. And if there are no timers, it will return -1 indicating infinity. */
}

/* 
Now you should have the answer to the question “Why do we block for I/O after executing any completed I/O callbacks? Shouldn’t Node be non-blocking?”……
The event loop will not be blocked if there are any pending tasks to be executed. If there are no pending tasks to be executed, it will only be blocked until the next timer goes off, which re-activates the loop. 

Now we know how long the loop should wait for any I/O to complete. This timeout value is then passed to uv__io_poll function.

==============================>>>>>>>>> This function will watch for any incoming I/O operations until this timeout expires or system-specified maximum safe timeout reaches. After the timeout, event loop will again become active and move on to the “check handlers” phase. <<<<<===========================

*/

/* Hàm uv_backend_timeout
If the loop’s stop_flag is set which determines the loop is about to exit, timeout is 0.
If there are no active handles or active operations pending, there’s no point of waiting, therefore the timeout is 0.
If there are pending idle handles to be executed, waiting for I/O should not be done. Therefore, the timeout is 0.
If there are completed I/O handlers in pending_queue, waiting for I/O should not be done. Therefore the timeout is 0.
If there are any close handlers pending to be executed, should not wait for I/O. Therefore, the timeout is 0. 

If none of the above criteria is met, uv__next_timeout method is called to determine how long libuv should wait for I/O.
What uv__next_timeout does is, it will return the value of the closest timer’s value. And if there are no timers, it will return -1 indicating infinity.

*/

// The following code is from https://github.com/libuv/libuv/blob/1ce6393a5780538ad8601cae00c5bd079b9415a9/src/unix/posix-poll.c
void uv__io_poll(uv_loop_t *loop, int timeout)
{
  sigset_t *pset;
  sigset_t set;
  uint64_t time_base;
  uint64_t time_diff;
  QUEUE *q;
  uv__io_t *w;
  size_t i;
  unsigned int nevents;
  int nfds;
  int have_signals;
  struct pollfd *pe;
  int fd;

  if (loop->nfds == 0)
  {
    assert(QUEUE_EMPTY(&loop->watcher_queue));
    return;
  }

  /* Take queued watchers and add their fds to our poll fds array.  */
  while (!QUEUE_EMPTY(&loop->watcher_queue))
  {
    q = QUEUE_HEAD(&loop->watcher_queue);
    QUEUE_REMOVE(q);
    QUEUE_INIT(q);

    w = QUEUE_DATA(q, uv__io_t, watcher_queue);
    assert(w->pevents != 0);
    assert(w->fd >= 0);
    assert(w->fd < (int)loop->nwatchers);

    uv__pollfds_add(loop, w);

    w->events = w->pevents;
  }

  /* Prepare a set of signals to block around poll(), if any.  */
  pset = NULL;
  if (loop->flags & UV_LOOP_BLOCK_SIGPROF)
  {
    pset = &set;
    sigemptyset(pset);
    sigaddset(pset, SIGPROF);
  }

  assert(timeout >= -1);
  time_base = loop->time;

  /* Loop calls to poll() and processing of results.  If we get some
   * results from poll() but they turn out not to be interesting to
   * our caller then we need to loop around and poll() again.
   */
  for (;;)
  {
    if (pset != NULL)
      if (pthread_sigmask(SIG_BLOCK, pset, NULL))
        abort();
    nfds = poll(loop->poll_fds, (nfds_t)loop->poll_fds_used, timeout);
    if (pset != NULL)
      if (pthread_sigmask(SIG_UNBLOCK, pset, NULL))
        abort();

    /* Update loop->time unconditionally. It's tempting to skip the update when
     * timeout == 0 (i.e. non-blocking poll) but there is no guarantee that the
     * operating system didn't reschedule our process while in the syscall.
     */
    SAVE_ERRNO(uv__update_time(loop));

    if (nfds == 0)
    {
      assert(timeout != -1);
      return;
    }

    if (nfds == -1)
    {
      if (errno != EINTR)
        abort();

      if (timeout == -1)
        continue;

      if (timeout == 0)
        return;

      /* Interrupted by a signal. Update timeout and poll again. */
      goto update_timeout;
    }

    /* Tell uv__platform_invalidate_fd not to manipulate our array
     * while we are iterating over it.
     */
    loop->poll_fds_iterating = 1;

    /* Initialize a count of events that we care about.  */
    nevents = 0;
    have_signals = 0;

    /* Loop over the entire poll fds array looking for returned events.  */
    for (i = 0; i < loop->poll_fds_used; i++)
    {
      pe = loop->poll_fds + i;
      fd = pe->fd;

      /* Skip invalidated events, see uv__platform_invalidate_fd.  */
      if (fd == -1)
        continue;

      assert(fd >= 0);
      assert((unsigned)fd < loop->nwatchers);

      w = loop->watchers[fd];

      if (w == NULL)
      {
        /* File descriptor that we've stopped watching, ignore.  */
        uv__platform_invalidate_fd(loop, fd);
        continue;
      }

      /* Filter out events that user has not requested us to watch
       * (e.g. POLLNVAL).
       */
      pe->revents &= w->pevents | POLLERR | POLLHUP;

      if (pe->revents != 0)
      {
        /* Run signal watchers last.  */
        if (w == &loop->signal_io_watcher)
        {
          have_signals = 1;
        }
        else
        {
          w->cb(loop, w, pe->revents);
        }

        nevents++;
      }
    }

    if (have_signals != 0)
      loop->signal_io_watcher.cb(loop, &loop->signal_io_watcher, POLLIN);

    loop->poll_fds_iterating = 0;

    /* Purge invalidated fds from our poll fds array.  */
    uv__pollfds_del(loop, -1);

    if (have_signals != 0)
      return; /* Event loop should cycle now so don't poll again. */

    if (nevents != 0)
      return;

    if (timeout == 0)
      return;

    if (timeout == -1)
      continue;

  update_timeout:
    assert(timeout > 0);

    time_diff = loop->time - time_base;
    if (time_diff >= (uint64_t)timeout)
      return;

    timeout -= time_diff;
  }
}

/* So, are timer callbacks executed during the poll phase or not?

No. timer callback are executed in the timer phase. However, the poll phase life cycle depends on the presence of timers in the loop. Example:

3 timers registered with 100 ms, 1000 ms, and 2000 ms respectively
one socket from which you want to read (that will take 5 seconds as the remote is not writing now)
As nothing ready at the moment when you enter the poll phase, the poller will have to take the minimum of 100, 1000 and 2000 as the timeout otherwise it will block for 5 seconds, and spoil the timer functions.
So the poll phase sets its timeout to 100ms, and polls.

Poll returns after 100ms, with no events (nfds = 0). I/O event is 5 seconds away.

So the poll phase returns.

As the uv loop (implemented by uv_run API) has loop in it, it goes back to the timer phase, and finding our first timer ready, fires it.

https://github.com/nodejs/help/issues/1118
 */