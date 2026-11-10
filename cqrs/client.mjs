import { accountCommand } from './accountCommand.mjs';
import { accountQuery } from './accountQuery.mjs';

accountCommand.addAccount("Dupont", "Jean");

const liste = accountQuery.getAccountList();