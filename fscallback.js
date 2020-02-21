const fs  = require("fs");

fs.readFile("text1.txt", function(err, dataMot){
    const file1 = dataMot.toString();
    fs.readFile("text2.txt", function(err, dataHai){
        const file2 = dataHai.toString();
        fs.readFile("text3.txt", function(err, dataBa){
            const file3 = dataBa.toString();
            const result = [file1, file2, file3];
            console.log(result);
        })

    })
})

