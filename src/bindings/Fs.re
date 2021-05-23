[@bs.module "fs"] external readFileSync: (string, string) => string;
[@bs.module "fs"] external existsSync: string => bool;
[@bs.module "fs"] external writeFileSync: (string, string) => unit;
[@bs.module "fs"] external mkdirSync: string => unit;
