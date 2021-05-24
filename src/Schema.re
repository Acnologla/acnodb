type watcherOps =
  | Set
  | Update
  | Delete;

module Schema = {
  type t('a) = {
    name: string,
    mutable watchers: list((string, 'a, watcherOps) => unit),
    data: Js.Dict.t('a),
  };
  let mut = (schema, key, value, watcherOp) =>
    List.iter(watcher => watcher(key, value, watcherOp), schema.watchers);
  let rec loop = (i, callback, values) =>
    if (i >= Array.length(values)) {
      None;
    } else {
      let (key, value) = values[i];
      callback(key, value) ? Some(value) : loop(i + 1, callback, values);
    };
  let rec loopKey = (i, callback, values) =>
    if (i >= Array.length(values)) {
      None;
    } else {
      let (key, value) = values[i];
      callback(key, value) ? Some(key) : loopKey(i + 1, callback, values)
    };
  let findKey = (schema, callback) =>
    Js.Dict.entries(schema.data) |> loopKey(0, callback);
  let find = (schema, callback) =>
    Js.Dict.entries(schema.data) |> loop(0, callback);
  let each = (schema, callback) =>
    Array.iter(callback, Js.Dict.entries(schema.data));
  let get = (schema, key) => Js.Dict.get(schema.data, key);
  let delete = (schema, key) => {
    Js.Dict.unsafeDeleteKey(. schema.data, key);
    mut(schema, key, Js.Dict.unsafeGet(schema.data, key), Delete);
  };
  let saveSchema = schema => {
    let name = schema.name;
    let path = Sys.getcwd() ++ {j|/data/$name.json|j};
    switch (Js.Json.stringifyAny(schema.data)) {
    | Some(value) => Fs.writeFileSync(path, value)
    };
  };
  let set = (~save=true, schema, key, value) => {
    switch (get(schema, key)) {
    | None =>
      Js.Dict.set(schema.data, key, value);
      mut(schema, key, value, Set);
    | _ =>
      Js.Dict.set(schema.data, key, value);
      mut(schema, key, value, Update);
    };
    if (save) {
      saveSchema(schema);
    };
  };
  let watch = (schema, callback) => {
    schema.watchers = [callback, ...schema.watchers];
  };
  [@bs.scope "JSON"] [@bs.val]
  external parse: string => Js.Dict.t('a) = "parse";
  let initSchema = (schema: t('a)) => {
    let name = schema.name;
    if (!Fs.existsSync(Sys.getcwd() ++ "/data")) {
      Fs.mkdirSync(Sys.getcwd() ++ "/data");
    };
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
  let schema = {name, data: Js.Dict.empty(), watchers: []};
  initSchema(schema);
  schema;
};
