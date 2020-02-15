const fs = require("fs");
const https = require("https");

console.log("1: \tSTART: HỌC CƠ BẢN"); //Synchronous code

setTimeout(() => {    //Timer
  console.log("2: \tsetTimeout 1", 0);
});

setImmediate(() => { //Checking
  console.log("3: \tsetImmediate");
});

https //I/O -> Polling using SYSTEM 
  .get("https://js.edu.vn/wp-json/", res => {
    console.log('4: \tI/O - Request - SYSTEM Polling, status:', res.statusCode);
    res.on("data", d => {});
  })
  .on("error", e => {
    console.error(e);
  });

  fs.readFile(__filename, () => { //I/O -> Polling using THREADPOOL 
  setTimeout(() => { //Vòng 3    
    console.log("5: \tsetTimeout trong readFile", 0);
  });
  setImmediate(() => { //Vòng 2    
    console.log("6: \tsetImmediate trong readFile");
  });
  process.nextTick(() => { //Vòng 2    
    console.log("7: \tNextTick trong readFile");
  });
});

Promise.resolve().then(() => { //MicroTask
  console.log("8: \tPromise main thread");
  process.nextTick(() => {
    console.log("9: \tNextTick trong Promise");
  });
});

queueMicrotask(function(){ //MicroTask
    console.log("10: \tThis is queueMicrotask")
})

process.nextTick(() => { //MicroTask
  console.log("11: \tNextTick main thread");
});

setTimeout(() => { //Timer
  console.log("12: \tsetTimeout 2", 0);
});

console.log("13: \tEND: https://youtube.com/hoccoban"); //Synchronous code


/* let result = 0;
for (let i = 0; i < 3000000000; i++) {
  result = result + i;
}
console.log("14: \t//Synchronous code blocking ", +result); */