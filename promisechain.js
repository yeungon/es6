function xayCaPhe() {
  return new Promise(function (resolve, reject) {
    setTimeout(function () {
      console.log('Bước 1: Chuẩn bị xay cà phê');
      resolve({cafe: ['Robusta', 'Arabica']});
      console.log('Bước 1: DONE');
    }, 2000);
  });
  
}
function vatSua(dataResolveMot) {
  const promise = new Promise(function (resolve, reject) {
    setTimeout(function () {
      console.log("Bước 2: Lấy sữa  + trộn với cà phê");
      resolve({newData: dataResolveMot.cafe[0] + ' + sữa ông thọ'});
      console.log('Bước 2: DONE');
    }, 2000);
  });
  return promise;
};
function phaCaPhe(dataResolveHai) {
  return new Promise(function (resolve, reject) {
    setTimeout(function () {
      console.log("Bước 3: Lấy sữa  + trộn với cà phê + pha cafe phê");
      resolve({result: dataResolveHai.newData + "+ pha bằng phin!"});
      console.log('Bước 3: DONE');
    }, 3000);
  });  
};
xayCaPhe().
then(vatSua).
then(phaCaPhe)
.then(function(cafePhaXong){
  console.log(cafePhaXong)
}); 

// async function done(){
//   const xay = await xayCaPhe();
//   const sua = await vatSua(xay);
//   const ketqua = await phaCaPhe(sua);
//   console.log(ketqua)
// }

// done()