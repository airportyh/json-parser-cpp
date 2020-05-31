for (let i = 0; i < 65535; i++) {
    console.log(i, ":", i.toString(16).padStart(4, "0"), ":", String.fromCharCode(i));
}
