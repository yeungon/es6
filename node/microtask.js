/* API của MicroTask thay đổi trên Node 11*/
function doWork(char) {
  console.log('promise', char);
}
const foo = [1, 2];
const bar = ['a', 'b'];

foo.forEach(num => {
  setImmediate(() => {
    console.log('setImmediate', num);
    bar.forEach(async (char) => {
      process.nextTick(() => {
        console.log('process.nextTick', char);
      });
       doWork(char);
    });
  });
});


//Trên Node >11 kết quả sẽ khác:

/* 
setImmediate 1
promise a
promise b
process.nextTick a
process.nextTick b
setImmediate 2
promise a
promise b
process.nextTick a
process.nextTick b
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