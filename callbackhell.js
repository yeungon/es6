/* setTimeout(function() {
  console.log("Bước 1: Chuẩn bị xay cà phê");
  const data = { cafe: ["Robusta", "Arabica"] };
  console.log("Bước 1: DONE");
  setTimeout(function() {
    console.log("Bước 2: Lấy sữa  + trộn với cà phê");
    const updatedData = { newData: data.cafe[0] + " + sữa ông thọ" };
    console.log("Bước 2: DONE");
    setTimeout(function() {
      console.log("Bước 3: Lấy sữa  + trộn với cà phê + pha cafe phê");
      const finalResult = { result: updatedData.newData + "+ pha bằng phin!" };
      console.log("Bước 3: DONE");
      setTimeout(function() {
        console.log(finalResult);
      }, 0);
    }, 3000);
  }, 2000);
}, 2000);
 */

function xayCaPhe() {
  setTimeout(function() {
    console.log("Bước 1: Chuẩn bị xay cà phê");
    const data = { cafe: ["Robusta", "Arabica"] };
    console.log("Bước 1: DONE");
    vatSua(data);        
  }, 2000);
}

function vatSua(data) {
  setTimeout(function() {
    console.log("Bước 2: Lấy sữa  + trộn với cà phê");
    const updatedData = { newData: data.cafe[0] + " + sữa ông thọ" };
    console.log("Bước 2: DONE");
    phaCaPhe(updatedData);
  }, 1000);
}
function phaCaPhe(updatedData) {
  setTimeout(function() {
    console.log("Bước 3: Lấy sữa  + trộn với cà phê + pha cafe phê");
    const finalResult = { result: updatedData.newData + "+ pha bằng phin!" };
    console.log("Bước 3: DONE");
    ketQua(finalResult);
  }, 500);
}

function ketQua(finalResult) {
  setTimeout(function() {
    console.log(finalResult);
  }, 0);
}


xayCaPhe();