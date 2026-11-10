import { randomUUID } from "node:crypto";

export class Account {
  constructor(id, lastName, firstName, creationDate) {
    this.id = id ?? randomUUID();
    this.lastName = lastName;
    this.firstName = firstName;
    this.creationDate = creationDate ?? new Date();
  }
}