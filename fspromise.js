const fs = require("fs");

function readPromise(file) {
  return new Promise(function(resolve, reject){
    fs.readFile(file, function(err, data) {
        resolve(data);
    });
  });  
  
}
let result = [];

readPromise("text1.txt")
.then(function(resolveData1){    
    const mot = resolveData1.toString();
    result.push(mot);
    return readPromise("text2.txt")    
})
.then(function(resolveData2){
    const hai = resolveData2.toString();
    result.push(hai);
    return readPromise("text3.txt")
})
.then(function(resolveData3){
    const ba = resolveData3.toString();
    result.push(ba);
    console.log(result);
})