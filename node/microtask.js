/* ::::::::::::::::::::::: Ví dụ 1 Thay đổi của MicroTask trên Node 11 :::::::::::::::::::::: */
//https://repl.it/@yeungon/promise
// Thảo luận https://github.com/nodejs/node/pull/22842
setTimeout(() => console.log("timeout1"));
setTimeout(() => {
  console.log("timeout2");
  process.nextTick(function() {
    console.log(`MicroTask - nextTick`);
  });
  Promise.resolve().then(() => console.log('promise resolve'))
});
setTimeout(() => console.log("timeout3"));
setTimeout(() => console.log("timeout4"));

/* Chạy trên Node < 11

timeout1
timeout2
timeout3
timeout4
MicroTask - nextTick
promise resolve
 */

//Trên Node >11, thứ tự sẽ là. Giống browser (Trừ nextTick) 

/* timeout1
timeout2
MicroTask - nextTick (trên browser không có nextTick)
promise resolve
timeout3
timeout4 */


/* ::::::::::::::::::::::: Ví dụ 2 Thay đổi của MicroTask trên Node 11 :::::::::::::::::::::: */
/* API của MicroTask thay đổi trên Node 11
https://repl.it/@yeungon/nexttick
*/
const foo = [1, 2];
const bar = ['a', 'b'];

foo.forEach(num => {
  setImmediate(() => {
    console.log('setImmediate', num);
    bar.forEach(async (char) => {      
       Promise.resolve().then(()=>{
           console.log("Promise: " + char)
       });

       //quản lý bởi nodejs, không phải của livuv
      process.nextTick(() => {
        console.log('process.nextTick', char);
      });

       //queueMicrotask chỉ có từ node11, managed by V8       
        /* queueMicrotask(() => {
           console.log("queueMicrotask: " + char)}
        );    */   
    });
  });
});

//Trên NODE < 11, chú ý queueMicrotask chưa có trên node < 11

/* setImmediate 1
setImmediate 2
process.nextTick a
process.nextTick b
process.nextTick a
process.nextTick b
Promise: a
Promise: b
Promise: a
Promise: b */

//Trên Node >11 kết quả sẽ khác:

/* 
setImmediate 1
process.nextTick a
process.nextTick b
Promise: a
Promise: b
setImmediate 2
process.nextTick a
process.nextTick b
Promise: a
Promise: b
 */


// on Node < 11
//https://repl.it/@yeungon/nexttick

/* setImmediate 1
promise a
promise b
setImmediate 2
promise a
promise b
process.nextTick a
process.nextTick b
process.nextTick a
process.nextTick b */