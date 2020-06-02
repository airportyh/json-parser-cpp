const fs = require("fs");

async function main() {
    const buffer = (await fs.promises.readFile("unicode.txt"))
    const chars = buffer.toString();
    console.log("buffer size:", buffer.length);
    console.log(buffer);
    for (let i = 0; i < buffer.length; i++) {
        const code = buffer[i];
        const char = String.fromCharCode(code);
        console.log(char + "\t" + code + "\t" + buffer[i].toString(2).padStart(8, "0"));
    }

    console.log("======================================");
    console.log("number of characters:", chars.length);
    console.log(chars);
    for (let i = 0; i < chars.length; i++) {
        const code = chars.charCodeAt(i);
        console.log(chars.charAt(i) + "\t" + code + "\t" + code.toString(2).padStart(16, "0"));
    }

    const realChars = Array.from(chars);
    console.log("======================================");
    console.log(JSON.stringify(realChars));
    console.log("number of real chars:", realChars.length);
    for (let char of realChars) {
        const codePoint = char.codePointAt(0);
        console.log(char + "\t" + codePoint + "\t" + codePoint.toString(2).padStart(32, "0"));
    }

}

main().catch(e => console.log(e.stack));