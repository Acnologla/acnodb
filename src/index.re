open Schema;

type userSchema = {
  username: string,
  x: int,
  tag: string,
};

type guildSchema = {
  name: string,
  owner: array(userSchema),
};

let users = newSchema("users");
let guilds: Schema.t(guildSchema) = newSchema("guilds");

let acno = Schema.find(users, (key, value) => value.username == "acno");
Js.log(acno);
