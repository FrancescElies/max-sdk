m = MyExternalJS();

result = m.doEvilThingsWith("parent");
longString = result[0];
patcherName = result[1];
numElementsPatcher = result[2];

    post("\nLast element long string " + longString.length + " chars, last char" + longString[32766]);
    post("\nPatcher name " + patcherName);
    post("\nNumber of elements in patcher " + numElementsPatcher);



