open Schema;

type entry('a) = {
  value: 'a,
  key: string,
};

let get = (json, key ) => 
    switch (Js.Dict.get(json, key)) {
    | Some(value) => value
    };

let createIndex = (schema, index) => {
  let data = Hashtbl.create(0);
  Schema.each(
    schema,
    ((key, value)) => {
      let json = Obj.magic(value);
      Hashtbl.add(data, get(json, index), [{value, key}]);
    },
  );
  Schema.watch(
    schema,
    (newKey, newValue, op) => {
      let json = Obj.magic(newValue);
      let indexKey = get(json, index);
      switch (Hashtbl.find(data, indexKey)) {
      | value =>
        switch (op) {
        | Delete =>
          let newList =
            List.filter(({key, value}) => key !== newKey, value);
          if (List.length(newList) == 0) {
            Hashtbl.remove(data, indexKey);
          } else {
            Hashtbl.replace(data, indexKey, newList);
          };
        | Set =>
          let newList = [{key: newKey, value: newValue}, ...value];
          Hashtbl.replace(data, indexKey, newList);
        | Update =>
          let newList =
            List.map(
              ({key, value}) =>
                key == newKey ? {key, value: newValue} : {key, value},
              value,
            );
          ();
        }
      | exception Not_found =>
        if (op == Set) {
          Hashtbl.add(data, get(json, index), [{value: newValue, key: newKey}]);
        }
      };
    },
  );
  data
};
