#include "Helper.h"

Helper::Helper()
{
}

Helper::~Helper()
{
}

string Helper::getCurrentDate()
{
  time_t rawtime;
  struct tm * timeinfo;
  char buffer [80];

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  strftime (buffer,80,"%x",timeinfo);
  return buffer;
}

string Helper::getTrendDate(simpleTrend aTrend, int dateOrder)
{
	for (int i = 0; i < int(aTrend.trend_dates.size()); i++)
		if (aTrend.trend_dates[i].order == dateOrder)
			return (Helper::getTrendDateHelper(aTrend.trend_dates[i]));
}

string Helper::getTrendDateHelper(trendDate aDate)
{
	string dateType = aDate.type;
	string myDate;
	if (dateType == "100")
		return aDate.year;
	else if (dateType == "011")
	{
		myDate = aDate.month;
		if (aDate.month.compare("0") == 0)
			return "Undefined";
		else
		{
			myDate.append("/");
			myDate.append(aDate.day);
			return myDate;
		}
	}
	else if (dateType == "110")
	{
		myDate = aDate.month;
		if (aDate.month.compare("0") == 0)
			return aDate.year;
		else
		{
			myDate.append("/");
			myDate.append(aDate.year);
			return myDate;
		}
	}
	else if (dateType == "111")
	{
		string myDate = aDate.month;
		myDate.append("/");
		myDate.append(aDate.day);
		myDate.append("/");
		myDate.append(aDate.year);
		return myDate;
	}
	else return "Undefined";
}

string Helper::getTrendName(string trendName)
{
	cout << trendName << endl;
	string newTrendName;
	if (trendName.compare(0, 3, "ris") == 0)
		newTrendName = "rising trend";
	else if (trendName == "bigjump")
		newTrendName = "big jump";
	else if (trendName.compare(0, 3, "sta") == 0)
		newTrendName = "stable trend";
	else if (trendName == "bigfall")
		newTrendName = "big fall";
	else if (trendName.compare(0, 4, "fall") == 0)
		newTrendName = "falling trend";
	else if (trendName == "NS")
		newTrendName = "non-sustained";
	else if (trendName == "S")
		newTrendName = "sustained";
	else if (trendName.compare(0, 5, "point") == 0)
		newTrendName = "point correlation";
	else newTrendName = "Undefined trend";
	return newTrendName;
}

string Helper::getSuggestionName(string category)
{
	if (category.compare("RT") == 0)
		return "rising trend";
	if (category.compare("FT") == 0)
		return "falling trend";
	if (category.compare("ST") == 0)
		return "stable trend";
	if (category.compare("CHT") == 0)
		return "changing trend";
	if (category.compare("CTR") == 0)
		return "changing trend that returns";
	if (category.compare("CTLS") == 0)
		return "trend that starts to change at the end";
	if (category.compare("CSCT") == 0)
		return "trend that changes and that starts to reverse at the end";
	if (category.compare("BJ") == 0)
		return "big jump";
	if (category.compare("BF") == 0)
		return "big fall";
	if (category.compare("PC") == 0)
		return "correlation between time points and values";
	
}



dateDetail Helper::getPeriods(string aDate1, string aDate2)
{
	dateDetail myDateDetail;
	int years, months, days;
	std::tr1::match_results<std::string::const_iterator> result1, result2;
	std::tr1::regex complete("(\\d+)(\\/)(\\d+)(\\/)(\\d+)");
	std::tr1::regex oneBar("(\\d+)(\\/)(\\d+)");
	std::tr1::regex none("(\\d+)");

	bool valid1Complete = std::tr1::regex_match(aDate1, result1, complete);
	bool valid1OneBar = std::tr1::regex_match(aDate1, result1, oneBar);
	bool valid1None = std::tr1::regex_match(aDate1, result1, none);
	bool valid2Complete = std::tr1::regex_match(aDate2, result2, complete);
	bool valid2OneBar = std::tr1::regex_match(aDate2, result2, oneBar);
	bool valid2None = std::tr1::regex_match(aDate2, result2, none);

	if(valid1Complete)
	{
		myDateDetail.initialMonth	= result1[1];
		myDateDetail.initialDay		= result1[3];
		myDateDetail.initialYear	= result1[5];
	}
	else if(valid1OneBar)
	{
		myDateDetail.initialMonth	= result1[1];
		myDateDetail.initialDay		= "0";	
		myDateDetail.initialYear	= result1[3];
	}
	else if(valid1None)
	{
		myDateDetail.initialMonth	= "0";
		myDateDetail.initialDay		= "0";	
		myDateDetail.initialYear	= result1[1];
	}
	if(valid2Complete)
	{
		
		myDateDetail.endMonth	= result2[1];
		myDateDetail.endDay		= result2[3];
		myDateDetail.endYear	= result2[5];
	}
	else if(valid2OneBar)
	{
		myDateDetail.endMonth	= result2[1];
		myDateDetail.endDay		= "0";
		myDateDetail.endYear	= result2[3];
	}
	else if(valid2None)
	{
		myDateDetail.endMonth	= "0";
		myDateDetail.endDay		= "0";	
		myDateDetail.endYear	= result2[1];
	}
	
	months	= atoi(myDateDetail.endMonth.c_str()) - atoi(myDateDetail.initialMonth.c_str());
	days	= atoi(myDateDetail.endDay.c_str()) - atoi(myDateDetail.initialDay.c_str());
	years	= atoi(myDateDetail.endYear.c_str()) - atoi(myDateDetail.initialYear.c_str());

	myDateDetail.numberMonths	= months;
	myDateDetail.numberDays		= days;
	myDateDetail.numberYears	= years;

	return myDateDetail;
}

//dateDetail Helper::getPeriods(string aDate1, string aDate2, string initialYear, string endYear, string initialMonth, string endMonth, string initialDay, string endDay)
//{
	//char * weekday[] = { "Sunday", "Monday",
	//				   "Tuesday", "Wednesday",
	//				   "Thursday", "Friday", "Saturday"};

	// prompt user for date 
	//printf ("Enter year: "); scanf ("%d",&year);
	//printf ("Enter month: "); scanf ("%d",&month);
	//printf ("Enter day: "); scanf ("%d",&day);

	// get current timeinfo and modify it to the user's choice
	//time ( &rawtime );
	//timeinfo = localtime ( &rawtime );
	//timeInfo->tm_year = year - 1900;
	//timeinfo->tm_mon = month - 1;
	//timeinfo->tm_mday = day;

	// call mktime: timeinfo->tm_wday will be set
	//mktime ( timeinfo );

	//printf ("That day is a %s.\n", weekday[timeinfo->tm_wday]);

	//return 0;
//}