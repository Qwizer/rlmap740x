CREATE TABLE "groups" (
	"id" SERIAL,
	"name" VARCHAR(255) NOT NULL,
	"flags" BIGINT NOT NULL DEFAULT 0,
	"access" INT NOT NULL DEFAULT 0,
	"violation" INT NOT NULL DEFAULT 0,
	"maxdepotitems" INT NOT NULL,
	"maxviplist" INT NOT NULL,
	PRIMARY KEY ("id")
);
INSERT INTO "groups" VALUES (1, 'Player', 0, 0, 0, 2000, 100),(2, 'Tutor', 16777216, 0, 0, 2000, 100),(3, 'Sennior Tutor', 274894684160, 0, 0, 2000, 100),(4, 'Community Manager', 69681547968463, 2, 0, 1000, 100),(5, 'Game Master', 69681547968463, 2, 0, 1000, 100),(6, 'GOD/OWNER', 57171953819640, 3, 0, 2000, 100);

CREATE TABLE "accounts" (
	"id" SERIAL,
	"name" VARCHAR(32) NOT NULL,
	"password" VARCHAR(255) NOT NULL,
	"email" VARCHAR(255) NOT NULL DEFAULT '',
	"premend" BIGINT NOT NULL DEFAULT 0,
	"blocked" SMALLINT NOT NULL DEFAULT 0,
	"warnings" SMALLINT NOT NULL DEFAULT 0,
	PRIMARY KEY ("id"),
	UNIQUE ("name")
);
INSERT INTO "accounts" VALUES (1, 'tibia', 'tibia', '', 0, 0, 0);

CREATE TABLE "players" (
	"id" SERIAL,
	"name" VARCHAR(255) NOT NULL,
	"account_id" INT NOT NULL,
	"group_id" INT NOT NULL,
	"sex" SMALLINT NOT NULL DEFAULT 0,
	"vocation" SMALLINT NOT NULL DEFAULT 0,
	"experience" BIGINT NOT NULL DEFAULT 0,
	"level" BIGINT NOT NULL DEFAULT 1,
	"maglevel" BIGINT NOT NULL DEFAULT 0,
	"health" BIGINT NOT NULL DEFAULT 100,
	"healthmax" BIGINT NOT NULL DEFAULT 100,
	"mana" BIGINT NOT NULL DEFAULT 100,
	"manamax" BIGINT NOT NULL DEFAULT 100,
	"manaspent" BIGINT NOT NULL DEFAULT 0,
	"soul" BIGINT NOT NULL DEFAULT 0,
	"direction" SMALLINT NOT NULL DEFAULT 0,
	"lookbody" BIGINT NOT NULL DEFAULT 10,
	"lookfeet" BIGINT NOT NULL DEFAULT 10,
	"lookhead" BIGINT NOT NULL DEFAULT 10,
	"looklegs" BIGINT NOT NULL DEFAULT 10,
	"looktype" BIGINT NOT NULL DEFAULT 136,
	"lookaddons" BIGINT NOT NULL DEFAULT 0,
	"posx" INT NOT NULL DEFAULT 0,
	"posy" INT NOT NULL DEFAULT 0,
	"posz" INT NOT NULL DEFAULT 0,
	"cap" INT NOT NULL DEFAULT 0,
	"lastlogin" BIGINT NOT NULL DEFAULT 0,
	"lastlogout" BIGINT NOT NULL DEFAULT 0,
	"lastip" BIGINT NOT NULL DEFAULT 0,
	"save" SMALLINT NOT NULL DEFAULT 1,
	"conditions" BYTEA NOT NULL,
	"skull_type" SMALLINT NOT NULL DEFAULT 0,
	"skull_time" BIGINT NOT NULL DEFAULT 0,
	"loss_experience" INT NOT NULL DEFAULT 100,
	"loss_mana" INT NOT NULL DEFAULT 100,
	"loss_skills" INT NOT NULL DEFAULT 100,
	"loss_items" INT NOT NULL DEFAULT 10,
	"loss_containers" INT NOT NULL DEFAULT 100
	"town_id" INT NOT NULL,
	"balance" INT NOT NULL DEFAULT 0,
	"stamina" INT NOT NULL DEFAULT 151200000,
	"online" SMALLINT NOT NULL DEFAULT 0,
	"rank_id" INT NOT NULL,
	"guildnick" VARCHAR(255) NOT NULL,
	PRIMARY KEY ("id"),
	UNIQUE ("name"),
	KEY ("online"),
	FOREIGN KEY ("account_id") REFERENCES "accounts" ("id") ON DELETE CASCADE,
	FOREIGN KEY ("group_id") REFERENCES "groups" ("id")
);

CREATE TABLE "guilds" (
	"id" SERIAL,
	"name" VARCHAR(255) NOT NULL,
	"owner_id" INT NOT NULL,
	"creationdate" INT NOT NULL,
	PRIMARY KEY ("id"),
	UNIQUE ("name"),
	FOREIGN KEY ("owner_id") REFERENCES "players" ("id") ON DELETE CASCADE
);

CREATE TABLE "guild_ranks" (
	"id" SERIAL,
	"guild_id" INT NOT NULL,
	"name" VARCHAR(255) NOT NULL,
	"level" INT NOT NULL,
	PRIMARY KEY ("id"),
	FOREIGN KEY ("guild_id") REFERENCES "guilds" ("id") ON DELETE CASCADE
);

CREATE TABLE "guild_members" (
	"player_id" INT NOT NULL,
	"rank_id" INT NOT NULL,
	"guild_nick" VARCHAR(255) NOT NULL DEFAULT '',
	UNIQUE ("player_id"),
	FOREIGN KEY ("player_id") REFERENCES "players" ("id") ON DELETE CASCADE,
	FOREIGN KEY ("rank_id") REFERENCES "guild_ranks" ("id") ON DELETE CASCADE
);

CREATE TABLE "guild_invites" (
	"player_id" INT NOT NULL,
	"guild_id" INT NOT NULL,
	
	UNIQUE ("player_id"),
	FOREIGN KEY ("player_id") REFERENCES "players" ("id") ON DELETE CASCADE,
	FOREIGN KEY ("guild_id") REFERENCES "guilds" ("id") ON DELETE CASCADE
);

CREATE TABLE "guild_wars" (
	"id" SERIAL,
	"guild_id" INT NOT NULL,
	"opponent_id" INT NOT NULL,
	"frag_limit" BIGINT NOT NULL DEFAULT 10,
	"declaration_date" BIGINT NOT NULL,
	"end_date" BIGINT NOT NULL,
	"guild_fee" BIGINT NOT NULL DEFAULT 1000,
	"opponent_fee" BIGINT NOT NULL DEFAULT 1000,
	"guild_frags" BIGINT NOT NULL DEFAULT 0,
	"opponent_frags" BIGINT NOT NULL DEFAULT 0,
	"comment" VARCHAR(255) NOT NULL DEFAULT '',
	"status" SMALLINT NOT NULL DEFAULT 0,
	PRIMARY KEY ("id"),
	FOREIGN KEY ("guild_id") REFERENCES "guilds" ("id") ON DELETE CASCADE,
	FOREIGN KEY ("opponent_id") REFERENCES "guilds" ("id") ON DELETE CASCADE
);	

CREATE TABLE "player_viplist" (
	"player_id" INT NOT NULL,
	"vip_id" INT NOT NULL,
	FOREIGN KEY ("player_id") REFERENCES "players" ("id") ON DELETE CASCADE,
	FOREIGN KEY ("vip_id") REFERENCES "players" ("id") ON DELETE CASCADE
);

CREATE TABLE "player_spells" (
	"player_id" INT NOT NULL,
	"name" VARCHAR(255) NOT NULL,
	FOREIGN KEY ("player_id") REFERENCES "players" ("id") ON DELETE CASCADE
);

CREATE TABLE "player_storage" (
	"player_id" INT NOT NULL,
	"key" INT NOT NULL,
	"value" INT NOT NULL,
	FOREIGN KEY ("player_id") REFERENCES "players" ("id") ON DELETE CASCADE
);

CREATE TABLE "player_skills" (
	"player_id" INT NOT NULL,
	"skillid" BIGINT NOT NULL,
	"value" BIGINT NOT NULL DEFAULT 0,
	"count" BIGINT NOT NULL DEFAULT 0,
	FOREIGN KEY ("player_id") REFERENCES "players" ("id") ON DELETE CASCADE
);

CREATE TABLE "player_items" (
	"player_id" INT NOT NULL,
	"sid" INT NOT NULL,
	"pid" INT NOT NULL DEFAULT 0,
	"itemtype" INT NOT NULL,
	"count" INT NOT NULL DEFAULT 0,
	"attributes" BYTEA,
	FOREIGN KEY ("player_id") REFERENCES "players" ("id") ON DELETE CASCADE,
	UNIQUE ("player_id", "sid")
);

CREATE TABLE "houses" (
	"id" SERIAL,
	"townid" INT NOT NULL DEFAULT 0,
	"name" VARCHAR(100) NOT NULL,
	"rent" INT NOT NULL DEFAULT 0,
	"guildhall" SMALLINT NOT NULL DEFAULT 0,
	"tiles" INT NOT NULL DEFAULT 0,
	"doors" INT NOT NULL DEFAULT 0,
	"beds" INT NOT NULL DEFAULT 0,
	"owner" INT NOT NULL DEFAULT 0,
	"paid" BIGINT NOT NULL DEFAULT 0,
	"clear" SMALLINT NOT NULL DEFAULT 0,
	"warnings" INT NOT NULL DEFAULT 0,
	"lastwarning" BIGINT NOT NULL DEFAULT 0,
	PRIMARY KEY ("id")
);

CREATE TABLE "house_auctions" (
	"id" SERIAL,
	"house_id" INT NOT NULL,
	"player_id" INT NOT NULL,
	"bid" INT NOT NULL DEFAULT 0,
	"limit" INT NOT NULL DEFAULT 0,
	"endtime" BIGINT NOT NULL DEFAULT 0,
	FOREIGN KEY ("house_id") REFERENCES "houses" ("id") ON DELETE CASCADE,
	FOREIGN KEY ("player_id") REFERENCES "players" ("id") ON DELETE CASCADE,
	PRIMARY KEY ("id")
);

CREATE TABLE "house_lists" (
	"house_id" INT NOT NULL,
	"listid" INT NOT NULL,
	"list" TEXT NOT NULL,
	FOREIGN KEY ("house_id") REFERENCES "houses" ("id") ON DELETE CASCADE
);

CREATE TABLE "bans" (
	"id" SERIAL,
	"type" BIGINT NOT NULL,
	"param" BIGINT NOT NULL,
	"active" SMALLINT DEFAULT 0,
	"expires" BIGINT NOT NULL,
	"added" BIGINT NOT NULL,
	"admin_id" INT,
	"comment" VARCHAR(1024) NOT NULL DEFAULT '',
	"reason" INT DEFAULT 0,
	"action" INT DEFAULT 0,
	"statement" VARCHAR(255) NOT NULL DEFAULT '',
	PRIMARY KEY ("id")
);

CREATE TABLE "tiles" (
	"id" SERIAL,
	"house_id" INT NOT NULL DEFAULT 0,
	"x" INT(6) NOT NULL,
	"y" INT(6) NOT NULL,
	"z" INT(3) NOT NULL,
	PRIMARY KEY("id"),
	FOREIGN KEY ("house_id") REFERENCES "houses" ("id") ON DELETE NO ACTION
);

CREATE TABLE "tile_items" (
	"tile_id" INT NOT NULL,
	"sid" INT NOT NULL,
	"pid" INT NOT NULL DEFAULT 0,
	"itemtype" INT NOT NULL,
	"count" INT NOT NULL DEFAULT 0,
	"attributes" BYTEA NOT NULL,
	FOREIGN KEY ("tile_id") REFERENCES "tiles" ("id") ON DELETE CASCADE
);

CREATE TABLE "map_store" (
	"house_id" INT NOT NULL,
	"data" BYTEA NOT NULL,
	KEY("house_id")
);

CREATE TABLE "player_deaths" (
	"id" SERIAL,
	"player_id" INT NOT NULL,
	"date" INT NOT NULL,
	"level" INT NOT NULL,
	PRIMARY KEY ("id"),
	FOREIGN KEY ("player_id") REFERENCES "players" ("id") ON DELETE CASCADE
);

CREATE TABLE "killers" (
	"id" SERIAL,
	"death_id" INT NOT NULL,
	"final_hit" SMALLINT NOT NULL DEFAULT 1,
	PRIMARY KEY("id"),
	FOREIGN KEY ("death_id") REFERENCES "player_deaths" ("id") ON DELETE CASCADE
);

CREATE TABLE "environment_killers" (
	"kill_id" INT NOT NULL,
	"name" VARCHAR(255) NOT NULL,
	PRIMARY KEY ("kill_id", "name"),
	FOREIGN KEY ("kill_id") REFERENCES "killers" ("id") ON DELETE CASCADE
);

CREATE TABLE "player_killers" (
	"kill_id" INT NOT NULL,
	"player_id" INT NOT NULL,
	"unjustified" SMALLINT NOT NULL DEFAULT 0,
	PRIMARY KEY ("kill_id", "player_id"),
	FOREIGN KEY ("kill_id") REFERENCES "killers" ("id") ON DELETE CASCADE,
	FOREIGN KEY ("player_id") REFERENCES "players" ("id") ON DELETE CASCADE
);

CREATE TABLE "player_depotitems" (
	"player_id" INT NOT NULL,
	"sid" INT NOT NULL,
	"pid" INT NOT NULL DEFAULT 0,
	"itemtype" INT NOT NULL,
	"count" INT NOT NULL DEFAULT 0,
	"attributes" BYTEA NOT NULL,
	FOREIGN KEY ("player_id") REFERENCES "players" ("id") ON DELETE CASCADE,
	UNIQUE ("player_id", "sid")
);

CREATE TABLE "global_storage" (
	"key" INT NOT NULL,
	"value" INT NOT NULL,
	PRIMARY KEY("key")
);

CREATE TABLE "schema_info" (
	"name" VARCHAR(255) NOT NULL,
	"value" VARCHAR(255) NOT NULL,
	PRIMARY KEY ("name")
);

INSERT INTO "schema_info" ("name", "value") VALUES ('version', 24);

CREATE FUNCTION "ondelete_accounts"()
RETURNS TRIGGER
AS $$
BEGIN
	DELETE FROM "bans" WHERE "type" = 3 AND "value" = OLD."id";
	RETURN OLD;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER "ondelete_accounts"
BEFORE DELETE
ON "accounts"
FOR EACH ROW
EXECUTE PROCEDURE "ondelete_accounts"();

CREATE FUNCTION "ondelete_players"()
RETURNS TRIGGER
AS $$
BEGIN
	DELETE FROM "bans" WHERE "type" = 2 AND "value" = OLD."id";
	UPDATE "houses" SET "owner" = 0 WHERE "owner" = OLD."id";

	RETURN OLD;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER "ondelete_players"
BEFORE DELETE
ON "players"
FOR EACH ROW
EXECUTE PROCEDURE "ondelete_players"();

CREATE FUNCTION "oncreate_guilds"()
RETURNS TRIGGER
AS $$
BEGIN
	INSERT INTO "guild_ranks" ("name", "level", "guild_id") VALUES ('Leader', 3, NEW."id");
	INSERT INTO "guild_ranks" ("name", "level", "guild_id") VALUES ('Vice-Leader', 2, NEW."id");
	INSERT INTO "guild_ranks" ("name", "level", "guild_id") VALUES ('Member', 1, NEW."id");

	RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER "oncreate_guilds"
AFTER INSERT
ON "guilds"
FOR EACH ROW
EXECUTE PROCEDURE "oncreate_guilds"();

CREATE FUNCTION "oncreate_players"()
RETURNS TRIGGER
AS $$
BEGIN
	INSERT INTO "player_skills" ("player_id", "skillid", "value") VALUES (NEW."id", 0, 10);
	INSERT INTO "player_skills" ("player_id", "skillid", "value") VALUES (NEW."id", 1, 10);
	INSERT INTO "player_skills" ("player_id", "skillid", "value") VALUES (NEW."id", 2, 10);
	INSERT INTO "player_skills" ("player_id", "skillid", "value") VALUES (NEW."id", 3, 10);
	INSERT INTO "player_skills" ("player_id", "skillid", "value") VALUES (NEW."id", 4, 10);
	INSERT INTO "player_skills" ("player_id", "skillid", "value") VALUES (NEW."id", 5, 10);
	INSERT INTO "player_skills" ("player_id", "skillid", "value") VALUES (NEW."id", 6, 10);

	RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER "oncreate_players"
AFTER INSERT
ON "players"
FOR EACH ROW
EXECUTE PROCEDURE "oncreate_players"();

INSERT INTO "players" VALUES (1, 'Administrator', 1, 6, 2, 1, 0, 1, 0, 185, 185, 35, 35, 0, 100, 2, 10, 10, 10, 10, 75, 0, 200, 200, 6, 435, 0, 0, 1, 1, '', 0, 0, 100, 100, 100, 10, 100, 1, 0, 151200000, 0, 0, '');
INSERT INTO "players" VALUES (2, 'Player', 1, 1, 1, 1, 0, 1, 0, 185, 185, 35, 35, 0, 100, 2, 10, 10, 10, 10, 75, 0, 200, 200, 6, 435, 0, 0, 1, 1, '', 0, 0, 100, 100, 100, 10, 100, 1, 0, 151200000, 0, 0, '');

# to add your own privileges for players/gms please use this flag generator http://hem.bredband.net/johannesrosen/playerflags.html