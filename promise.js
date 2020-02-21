/* Để hiểu promise 
1) Promise là một dạng class, hoặc function constructor có sẵn (natively, built-in)

Để dùng, ta tạo một object từ class/function costructor Promise đó.

2) Ta bắt buộc phải truyền vào Promise một hàm, hàm là là tham số dạng constructor của class/Function constructor Promise, hàm này gọi là callback. Hàm callback này có 2 tham số, thường được viết là resolve và reject, nhưng có thể là bất cứ tên gì --> tượng trưng. Vì trong callback - patter của JavaScript, thì hàm callback được thực thi lúc viết hàm chứa nó, và hàm callback được viết khi gọi hàm chứa nó.

3) Bước 3, ta tạo một hàm bọc lấy object đó để tái sử dụng. 

4) Khi sử dụng, ta có thể dùng .then là method của promise object. Then này có thể chain (nối thành chuỗi) ==> Do đặc tính chaining của JavaScript. Ví dụ về hàm chaining.


*/

/*function callBack(resolve, reject){

}

const a = new Promise(callBack);

console.log(a)

*/

function getPromise(flag){

	const result = new Promise(function(yes, no){
		if(flag === true){
			yes()
		}else{
			no()
		}
	});

	return result;   
}

getPromise(true)
.then(function(){
	console.log ("thanh cong")
})
.catch(function(){
	console.log("that bai")
})


getPromise(false)
.then(function(){
	console.log ("thanh cong")
})
.catch(function(){
	console.log("that bai")
})