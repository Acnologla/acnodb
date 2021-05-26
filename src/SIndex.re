open Schema;

let get = (json, key) => Js.Option.getExn(Js.Dict.get(json, key));

let rec update = (data, index, newKey, indexKey, oldValue, op) =>
  switch (Hashtbl.find(data, indexKey)) {
  | value =>
    switch (op) {
    | Delete =>
      let newList = List.filter(key => key !== newKey, value);
      if (List.length(newList) == 0) {
        Hashtbl.remove(data, indexKey);
      } else {
        Hashtbl.replace(data, indexKey, newList);
      };
    | Set =>
      let newList = [newKey, ...value];
      Hashtbl.replace(data, indexKey, newList);
    | Update =>
      let oldVal = Obj.magic(oldValue)->get(index);
      if (oldVal != indexKey) {
        update(data, index, newKey, oldVal, oldValue, Delete);
        update(data, index, newKey, indexKey, oldValue, Set);
      };
    }
  | exception Not_found =>
    if (op == Set) {
      Hashtbl.add(data, indexKey, [newKey]);
    }
  };

let createIndex = (schema, index) => {
  let data = Hashtbl.create(0);
  Schema.each(
    schema,
    ((key, value)) => {
      let json = Obj.magic(value);
      update(data, index, key, get(json, index), None, Set);
    },
  );
  Schema.watch(
    schema,
    (newKey, newValue, oldValue, op) => {
      let json = Obj.magic(newValue);
      let indexKey = get(json, index);
      update(data, index, newKey, indexKey, oldValue, op);
    },
  );
  data;
};
