const s = "I am 何宇翔😷. I like π. To ∞ and beyond!🎢";

const b1 = Buffer.from(s, "utf8");
console.log(b1);
const b2 = Buffer.from(s, "utf-16le");
console.log(b2);