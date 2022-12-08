
/**
 * @brief Finds a character in a string
 * 
 * @param str 
 * @param c 
 * @return pointer to the character or NULL if not found 
 */
char *findChar(char *str, char c)
{
    while (*str != c)
    {
        if (*str == '\0')
        {
            return NULL;
        }
        str++;
    }
    return str;
}

/**
 * @brief Get the Json Value that corresponds to the given key
 * 
 * @param jsonString 
 * @param key 
 * @param valueOut 
 * @return 0 if success, -1 if failure
 */
int getJsonValue(char *jsonString, char *key, char *valueOut)
{
  // find the value substring 
  char *p = strstr(jsonString, key);
  if (p == NULL)
  {
    return -1;
  }
  p = findChar(p, ':');
  if (p == NULL)
  {
    return -1;
  }
  p = findChar(p, '"');
  if (p == NULL)
  {
    return -1;
  }
  p++;
  char *p2 = findChar(p, '"');
  if (p2 == NULL)
  {
    return -1;
  }
  int len = p2 - p;
  strncpy(valueOut, p, len);
  valueOut[len] = '\0';
  return 0;
}

/**
 * @brief Extracts the time from an ISO 8601 string
 * 
 * @note: The integer size is 32 bits!
 
 * @param iso8601 
 * @return uint32_t 
 */
uint32_t iso8601ToInt(char *iso8601) 
{
  uint32_t time = 0;
  char *p = iso8601;
  while (*p != 'T') 
    p++;
  p++;
  while (*p != ':') 
    time = time * 10 + (*p++ - '0');
  p++;
  while (*p != ':') 
    time = time * 10 + (*p++ - '0');
  p++;
  while (*p != '-') 
    time = time * 10 + (*p++ - '0');
    
  return time;
}