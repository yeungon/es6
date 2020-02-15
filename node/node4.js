const fs = require("fs");

const start = Date.now();

function thoiGian(bandau) {
  return Date.now() - bandau;
}

setTimeout(() => {
  const lag = thoiGian(start);
  console.log(`Timer: \tSetTimeout: \tThời gian chạy: \t${lag} ms`);
}, 120);

setImmediate(function() {
  const lag = thoiGian(start);
  console.log(`Checking: \tsetImmediate: \tThời gian chạy: \t${lag} ms`);
});

function readFile() {
  fs.readFile("text.txt", (e, data) => {
    let result = 0;
    for (let i = 0; i < 300000000; i++) {
      result = result + i;
    }
    const lag = thoiGian(start);
    console.log(`I/O \treadFile: \tThời gian chạy: \t${lag} ms`);
  });
}

readFile();

