# ACNODB

A simple json database (document oriented) reason database

## Installation

```
npm i acnodb
```

Then add acnodb as a dependency to bsconfig.json:

```
"bs-dependencies": [
+  "rescript-hash"
]
```

## Usage
```re
open Schema;
type userSchema = {
  username: string,
  x: int,
  tag: string,
};

let users = newSchema("users");

//set
Schema.set(users, "acno", {
    username: "y",
    x: 0,
    tag: "xxx"
});

//watch changes
Schema.watch(users, (key, value, oldValue, operation) => 
  switch(operation){
    | Set => Js.log("Set")
    | Update => Js.log("Update")
    | Delete => Js.log("Delete")
  }
)

//get
Schema.get(users, "acno");

//remove

Schema.delete(users, "acno");

//find by value
let u = Schema.find(users, (key, value) =>  value.username == "k");
Js.log(acno);

//Create indexes
let userIndex = SIndex.createIndex(users, "username");

//Get value from index
let uKeys = Hashtbl.find(userIndex, "acno")

```