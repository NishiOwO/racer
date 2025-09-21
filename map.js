const perlin = require("./perlin.js");

const wid = 64;
const step = 0.25;
let count = 0;

console.log(wid + " " + step);

perlin.seed(Math.random());
for(let y = 0; y < wid; y += step){
	for(let x = 0; x < wid; x += step){
		const n = (perlin.simplex2(x / (wid / 8), y / (wid / 8)) + 1) / 2;
		console.log(n);
		count++;
	}
}
