const s = Date.now();

//A: Code đồng bộ tác vụ tốn thời gian (3.5s)
let result = 0;
for (let i = 0; i < 3000000000; i++) {
  result = result + i;
}

//B: Code đồng bộ sẽ bị chặn, chạy sau A
console.log(`Sync: ${result}-${Date.now()-s}`);


//C: Code bất đồng bộ
setTimeout(function() {
  console.log(`setTimeout 1st:${Date.now()-s}`);
}, 1000);

//D: Code bất đồng bộ
setTimeout(function() {
  console.log(`setTimeout 2nd:${Date.now()-s}`);
}, 3000);



//https://repl.it/@yeungon/async
