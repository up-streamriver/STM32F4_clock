#include "weather.h"

bool parse_seniverse_response(const char *response,weather_info_t *info)
{
    const char *result_response = strstr(response,"\"results\":");
    if(result_response == NULL)
        return false;
    const char *location_response = strstr(result_response,"\"location\":");
    if(location_response == NULL)
        return false;
    const char *city_response = strstr(location_response,"\"name\":");
    if(city_response)
    {
        sscanf(city_response,"\"name\": \"%31[^\"]\"",info->city);
    }
    const char *path_response = strstr(city_response,"\"path\":");
    if(path_response)
    {
        sscanf(path_response,"\"path\": \"%128[^\"]\"",info->location);
    }
    const char *now_response = strstr(path_response,"\"now\":");
    if(now_response == NULL)
        return false;
    const char *text_response = strstr(now_response,"\"text\":");
    if(text_response)
    {
        sscanf(text_response,"\"text\": \"%15[^\"]\"",info->weather);
    }    
    const char *code_response = strstr(text_response,"\"code\":");
    if(code_response)
    {
        sscanf(code_response,"\"text\": \"%d\"",info->weather_code);
    } 
    char temperature_str[16] = { 0 };
    const char *temp_response = strstr(code_response,"\"temperature\":");
    if(temp_response)
    {
       if(sscanf(temp_response,"\"temperature\": \"%15[^\"]\"",temperature_str) == 1)
       {
            info->temperature = atof(temperature_str);
       } 
    }          
    return true;
}