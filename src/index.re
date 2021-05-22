open Schema;

type userSchema = {
  username: string,
  x: int,
  tag: string,
};

let users = newSchema("users");
let acno = Schema.find(users, (key, value) => value.username == "acno");
Js.log(acno);
