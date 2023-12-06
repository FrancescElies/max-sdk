var a = new MyExternalJS();

// trigger the print method of myexternaljs
function bang() {
	a.print();
}

function set(val) {
	a.myattr = val; // set the value of myattr
}

// output the contents of myattr
function get() {
	outlet(0, a.myattr);
}

// sends a value to myexternaljs and gets the result back
function abs(val) {
	outlet(0, a.doAbs(val));
}
