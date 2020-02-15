/* ::::::::::::::::::::::::::::::::::::::::::::1: sync vs async:::::::::::::::::::::::::::::::::::::::::::: */

const start = Date.now();
const fs = require("fs");

function thoiGian(bandau) {
  return Date.now() - bandau;
}

setTimeout(() => {
  const lag = thoiGian(start);
  console.log(
    `Timer:\tSetTimeout thứ nhất -  0 giây:\tThời gian chạy:\t${lag} ms`
  );
}, 0);

fs.readFile(__filename, () => {  
  console.log("I/O callback: " + __filename);
});

setImmediate(function() {
  const lag = thoiGian(start);
  console.log(`Check:\tsetImmediate:\tThời gian chạy:\t${lag} ms`);
});



/* let result = 0;
for (let i = 0; i < 3000000000; i++) {
  result = result + i;
}

console.log("SYNCHRONOUS/BLOCKING code ", +result);
 */



/* ::::::::::::::::::::::::::::::::::::::::::::2 Timer và Check phrase trong I/O, Timer chạy ở vòng 2:::::::::::::::::::::::::::::::::::::::::::: */

/* const fs = require("fs");

const start = Date.now();
function thoiGian(bandau) {
  return Date.now() - bandau;
}

fs.readFile(__filename, () => {
  setTimeout(() => {
    const lag = thoiGian(start);
    console.log(
      `Timer:\tSetTimeout thứ nhất -  0 giây:\tThời gian chạy:\t${lag} ms`
    );
  }, 0);

  setTimeout(() => {
    const lag = thoiGian(start);
    console.log(
      `Timer:\tSetTimeout thứ hai - 0 giây:\tThời gian chạy:\t${lag} ms`
    );
  }, 0);

  setImmediate(function() {
    const lag = thoiGian(start);
    console.log(`Check:\tsetImmediate:\tThời gian chạy:\t${lag} ms`);
  });

  console.log("Filename: " + __filename);
});

 */

 

/* ::::::::::::::::::::::::::::::::::::::::::::3: Ví dụ 2 Thay đổi của MicroTask trên Node 11 :::::::::::::::::::::::::::::::::::::::::::: */
/* 
setTimeout(() => console.log("timeout1"));
setTimeout(() => {
  console.log("timeout2");
  //từ node11, kết quả đã giống với browser (chú ý browser không có nextTick)
  process.nextTick(function() {
    console.log(`MicroTask - nextTick`);
  });
  Promise.resolve().then(() => console.log("promise resolve"));
});
setTimeout(() => console.log("timeout3"));
setTimeout(() => console.log("timeout4")); */

//Node < 11 https://repl.it/@yeungon/promise


/* ::::::::::::::::::::::::::::::::::::::::::::4: Thứ tự của MicroTask vs MacroTask và 3 vòng Event Loop :::::::::::::::::::::::::::::::::::::::::::: */ 