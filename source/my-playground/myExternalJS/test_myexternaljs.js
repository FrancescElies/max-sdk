m = MyExternalJS();

result = m.doEvilThingsWith("parent")[0];

post("result: " + result.slice(0, 10));
post("Returned str length " + result.length + "\n");
