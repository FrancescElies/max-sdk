var myexternal = MyExternalJS();
// trigger the print method of myexternaljs
function bang() {
    post("START\n");
    post(this.patcher);
    myexternal.doEvilThingsWith(1);
    myexternal.doEvilThingsWith(this.patcher);
    post("END\n");
}
