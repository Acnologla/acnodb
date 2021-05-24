type watcherOps =
  | Set
  | Update
  | Delete;

let (<$>) = func =>
  fun
  | Some(x) => Some(func(x))
  | None => None;

module Schema = {
  type watcher('a) = {
    callback: (string, 'a, watcherOps) => unit,
    path: string,
  };
  type t('a) = {
    name: string,
    mutable watchers: list(watcher('a)),
    data: Js.Dict.t('a)
  };
  let mut = (schema, key, value, watcherOp) =>
    List.iter(
      watcher =>
        switch (watcher.path) {
        | "*" => watcher.callback(key, value, watcherOp)
        | path => path == key ? watcher.callback(key, value, watcherOp) : ()
        },
      schema.watchers,
    );
  let rec findByFunc = (cb, schema) => {
    let rec loop = (i, values) =>
      switch (i >= Array.length(values)) {
      | true => None
      | _ =>
        let (key, value) = values[i];
        cb(key, value) ? Some((key, value)) : loop(i + 1, values);
      };
    loop(0, Js.Dict.entries(schema.data));
  };

  let findKey = (schema, callback) => fst <$> findByFunc(callback, schema);
  let find = (schema, callback) => snd <$> findByFunc(callback, schema);
  let each = (schema, callback) =>
    Array.iter(callback, Js.Dict.entries(schema.data));
  let get = (schema, key) => Js.Dict.get(schema.data, key);
  let saveSchema = schema => {
    let name = schema.name;
    let path = Sys.getcwd() ++ {j|/data/$name.json|j};
    switch (Js.Json.stringifyAny(schema.data)) {
    | Some(value) => Fs.writeFileSync(path, value)
    };
  };
  let watcherExists = (schema, path) =>
    List.exists(watcher => watcher.path == path, schema.watchers);
  let delete = (schema, key) => {
    Js.Dict.unsafeDeleteKey(. schema.data, key);
    mut(schema, key, Js.Dict.unsafeGet(schema.data, key), Delete);
    saveSchema(schema);
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
  let watch = (~path="*", schema, callback) => {
    schema.watchers = [{callback, path}, ...schema.watchers];
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
