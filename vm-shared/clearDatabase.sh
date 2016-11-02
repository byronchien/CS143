#!/bin/bash

mysql TEST < drop.sql;
mysql TEST < create.sql;
mysql TEST < load.sql;