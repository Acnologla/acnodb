# ACNODB

A simple json database (document oriented) reason database

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

//get
Schema.get(users, "acno");

//remove

Schema.delete(users, "acno");

//find by value
let acno = Schema.find(users, (key, value) => value.username == "acno");
Js.log(acno);
```