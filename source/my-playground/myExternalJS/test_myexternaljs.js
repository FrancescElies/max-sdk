var m = new MyExternalJS();

// trigger the print method of myexternaljs
function bang() {
    post("START\n");
    m.print();
    post(this.patcher);
    outlet(0, m.doEvilThingsWith(1));
    outlet(0, m.doEvilThingsWith(this.patcher));
    post("END\n");
}
