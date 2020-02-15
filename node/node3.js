const start = Date.now();
const crypto = require("crypto");
const fs = require("fs");
const https = require("https");

//process.env.UV_THREADPOOL_SIZE = 4;

https
  .get("https://google.co.nz/", res => {
    res.on("data", d => {});
    res.on("end", () => {
      const end = Date.now();
      const tong = end - start;
      console.log(`HTTPS Request-\t:${tong}`);
    });
  })
  .on("error", e => {
    console.error(e);
  });

function read() {
  fs.readFile(__filename, function(err, data) {
    const end = Date.now();
    const tong = end - start;
    console.log(`THREADPOOL-fs-\t${tong}`);
  });
}

function theHash() {
   crypto.pbkdf2("hocoban", "https://youtube.com/hoccoban", 1, 1024, "sha512", function() {
    const end = Date.now();
    const tong = end - start;
    console.log(`THREADPOOL-crypto ASYNC-:\t${tong}`);
  });
  
}

read();     //
//theHash();  //==> read()
//theHash();  //==> thread3
//theHash();  //==> thread4
//theHash(); //==> thread1

