module Schema = {
  type t('a) = {
    name: string,
    data: Hashtbl.t(string, 'a),
  };
  let findKey = (schema, callback) =>
    Hashtbl.fold(
      (key, value, acc) =>
        switch (acc) {
        | Some(_) => acc
        | None => callback(key, value) ? Some(key) : None
        },
      schema.data,
      None,
    );
  let find = (schema, callback) =>
    Hashtbl.fold(
      (key, value, acc) =>
        switch (acc) {
        | Some(_) => acc
        | None => callback(key, value) ? Some(value) : None
        },
      schema.data,
      None,
    );
  let delete = (schema, key) => Hashtbl.remove(schema.data, key);
  let each = (schema, callback) => Hashtbl.iter(callback, schema.data);
  let get = (schema, key) => Hashtbl.find(schema.data, key);
  let saveSchema = schema => {
    let name = schema.name;
    let path = Sys.getcwd() ++ {j|/data/$name.json|j};
    let json = Js.Dict.empty();
    Hashtbl.iter(
      (key, value) => Js.Dict.set(json, key, value),
      schema.data,
    );
    switch (Js.Json.stringifyAny(json)) {
    | Some(value) => Fs.writeFileSync(path, value)
    };
  };
  let set = (~save=true, schema, key, value) => {
    switch (Hashtbl.find(schema.data, key)) {
    | exception Not_found => Hashtbl.add(schema.data, key, value)
    | _ => Hashtbl.replace(schema.data, key, value)
    };
    if (save) {
      saveSchema(schema);
    };
  };
  [@bs.scope "JSON"] [@bs.val]
  external parse: string => Js.Dict.t('a) = "parse";
  let initSchema = (schema: t('a)) => {
    let name = schema.name;
    let path = Sys.getcwd() ++ {j|/data/$name.json|j};
    if (Fs.existsSync(path)) {
      Fs.readFileSync(path, "utf-8")
      |> parse
      |> Js.Dict.entries
      |> Array.iter(((key, value)) => set(~save=false, schema, key, value));
      ();
    } else {
      Fs.writeFileSync(path, "{}");
    };
  };
  let getAndUpdate = (schema, key, callback) =>
    get(schema, key) |> callback |> set(schema, key);
};

open Schema;
let newSchema = name => {
  let schema = {name, data: Hashtbl.create(0)};
  initSchema(schema);
  schema;
};
