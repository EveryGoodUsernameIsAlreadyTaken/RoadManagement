---------------------------------------------------------------------------------------
-- [PROCEDURE]: PQ_INSERT_CITY
-- INSERT A ROW INTO CITY
---------------------------------------------------------------------------------------
IF EXISTS(SELECT * FROM SYS.PROCEDURES WHERE NAME = 'PQ_INSERT_CITY' AND TYPE = 'P' )
  DROP PROC PQ_INSERT_CITY
GO
CREATE PROC PQ_INSERT_CITY
	@CITYID			INT,
	@CITYNAME		VARCHAR(30)
AS
BEGIN
	INSERT INTO CITIES VALUES (@CITYID, @CITYNAME)
END
GO

---------------------------------------------------------------------------------------
-- [PROCEDURE]: PQ_GET_CITYNAME_WITH_ID
-- SELECTS THE NAME OF CITY WITH AN ID
---------------------------------------------------------------------------------------
IF EXISTS(SELECT * FROM SYS.PROCEDURES WHERE NAME = 'PQ_GET_CITYNAME_WITH_ID' AND TYPE = 'P' )
  DROP PROC PQ_GET_CITYNAME_WITH_ID
GO
CREATE PROC PQ_GET_CITYNAME_WITH_ID
	@CITYID			INT
AS
BEGIN
	SELECT CITYNAME FROM CITIES WHERE CITYID = @CITYID
END
GO


---------------------------------------------------------------------------------------
-- [PROCEDURE]: PQ_INSERT_ROAD_TRAFFIC
-- INSERT A ROW INTO ROADTRAFFICINFO
---------------------------------------------------------------------------------------
IF EXISTS(SELECT * FROM SYS.PROCEDURES WHERE NAME = 'PQ_INSERT_ROAD_TRAFFIC' AND TYPE = 'P' )
  DROP PROC PQ_INSERT_ROAD_TRAFFIC
GO
CREATE PROC PQ_INSERT_ROAD_TRAFFIC
	@CITYFROMID			INT,
	@CITYTOID			INT,
	@DRIVEDIST			INT,
	@DIRECTDIST			INT,
	@SPEEDLIMIT			INT,
	@WEATHERCONDITION	INT,
	@TRAFFICCONDITION	INT,
	@BLOCKED			BIT,
	@TIME				VARCHAR(30)
AS
BEGIN
	INSERT INTO ROADTRAFFICINFO VALUES (@CITYFROMID, @CITYTOID, @DRIVEDIST, @DIRECTDIST, @SPEEDLIMIT, @WEATHERCONDITION, @TRAFFICCONDITION, @BLOCKED, @TIME)
END
GO

SELECT * FROM ROADTRAFFICINFO

---------------------------------------------------------------------------------------
-- [PROCEDURE]: PQ_SELECT_FROM_RTINFO
-- SELECTS ALL FROM ROAD TRAFFIC INFO
---------------------------------------------------------------------------------------
IF EXISTS(SELECT * FROM SYS.PROCEDURES WHERE NAME = 'PQ_SELECT_FROM_RTINFO' AND TYPE = 'P' )
  DROP PROC PQ_SELECT_FROM_RTINFO
GO
CREATE PROC PQ_SELECT_FROM_RTINFO
AS
BEGIN
	SELECT * FROM ROADTRAFFICINFO
END
GO

---------------------------------------------------------------------------------------
-- [PROCEDURE]: PQ_SELECT_ROWS_FROM_RTINFO
-- SELECTS FROM ROAD TRAFFIC INFO WITH MATCHING TIME
---------------------------------------------------------------------------------------
IF EXISTS(SELECT * FROM SYS.PROCEDURES WHERE NAME = 'PQ_SELECT_ROWS_FROM_RTINFO' AND TYPE = 'P' )
  DROP PROC PQ_SELECT_ROWS_FROM_RTINFO
GO
CREATE PROC PQ_SELECT_ROWS_FROM_RTINFO
	@TIME		VARCHAR(30)
AS
BEGIN
	SELECT * FROM ROUTEINFO;
END
GO

SELECT * FROM ROUTEINFO
EXEC PQ_SELECT_ROWS_FROM_RTINFO '%53:24%'

---------------------------------------------------------------------------------------
-- [PROCEDURE]: PQ_INSERT_FAST_ROUTE
-- INSERTS INTO ROUTEINFO
---------------------------------------------------------------------------------------
IF EXISTS(SELECT * FROM SYS.PROCEDURES WHERE NAME = 'PQ_INSERT_FAST_ROUTE' AND TYPE = 'P' )
  DROP PROC PQ_INSERT_FAST_ROUTE
GO
CREATE PROC PQ_INSERT_FAST_ROUTE
	@TIME				VARCHAR(30),
	@CITY2				VARCHAR(30),
	@TIME2				INT,
	@CITY3				VARCHAR(30),
	@TIME3				INT,
	@CITY4				VARCHAR(30),
	@TIME4				INT,
	@TIME5				INT
AS
BEGIN
	INSERT INTO ROUTEINFO VALUES(@TIME, 'LA', 0, @CITY2, @TIME2, @CITY3, @TIME3, @CITY4, @TIME4, 'NY', @TIME5)
END
GO

EXEC PQ_INSERT_FAST_ROUTE '0:27:00', 'Oklahoma City', 5, 'St. Louis', 11, 'Chicago', 18, 40
--SELECT * FROM CITIES
--SELECT * FROM ROADTRAFFICINFO
--SELECT * FROM ROUTEINFO
--EXEC PQ_SELECT_ROWS_FROM_RTINFO '11:38%'