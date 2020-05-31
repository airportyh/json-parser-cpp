const fs = require("fs");

async function main() {
    const buffer = (await fs.promises.readFile("unicode.txt"))
    const chars = buffer.toString();
    console.log("buffer size:", buffer.length);
    console.log(buffer);
    for (let i = 0; i < buffer.length; i++) {
        console.log(i, ":", buffer[i]);
    }

    console.log("number of characters :", chars.length);
    console.log(chars);
    for (let i = 0; i < chars.length; i++) {
        console.log(i, ":", chars.charAt(i));
    }
}

main().catch(e => console.log(e.stack));