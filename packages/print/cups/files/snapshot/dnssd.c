//
// DNS-SD stubs for CUPS 2.5 when --with-dnssd=no on Linux.
//
// Upstream dnssd.c falls through to Avahi when neither HAVE_MDNSRESPONDER nor
// _WIN32 is set, so --with-dnssd=no still requires avahi-client headers.
// OpenWrt snapshot builds replace this file instead of pulling in Avahi.
//

#include "cups-private.h"
#include "dnssd.h"
#include <ctype.h>

struct _cups_dnssd_s
{
  cups_rwlock_t		rwlock;
  size_t		config_changes;
  cups_dnssd_error_cb_t	cb;
  void			*cb_data;
};

struct _cups_dnssd_browse_s
{
  cups_dnssd_t		*dnssd;
};

struct _cups_dnssd_query_s
{
  cups_dnssd_t		*dnssd;
};

struct _cups_dnssd_resolve_s
{
  cups_dnssd_t		*dnssd;
};

struct _cups_dnssd_service_s
{
  cups_dnssd_t		*dnssd;
  char			*name;
};

bool
cupsDNSSDAssembleFullName(
    char       *fullname,
    size_t     fullsize,
    const char *name,
    const char *type,
    const char *domain)
{
  char		*fullptr,
		*fullend;

  if (!fullname || !fullsize || !name || !type)
    return (false);

  for (fullptr = fullname, fullend = fullname + fullsize - 1; *name; name++)
  {
    if (*name == ' ' || *name == '\\' || (*name & 0x80))
    {
      if ((fullend - fullptr) < 4)
        return (false);

      snprintf(fullptr, (size_t)(fullend - fullptr + 1), "\\%03d", *name & 255);
      fullptr += strlen(fullptr);
    }
    else
    {
      if (fullptr > fullend)
        return (false);

      *fullptr++ = *name;
    }
  }

  snprintf(fullptr, (size_t)(fullend - fullptr + 1), ".%s.%s", type, domain ? domain : "local.");

  return (true);
}

void
cupsDNSSDBrowseDelete(cups_dnssd_browse_t *browse)
{
  if (!browse)
    return;

  free(browse);
}

cups_dnssd_t *
cupsDNSSDBrowseGetContext(cups_dnssd_browse_t *browse)
{
  return (browse ? browse->dnssd : NULL);
}

cups_dnssd_browse_t *
cupsDNSSDBrowseNew(
    cups_dnssd_t *dnssd,
    uint32_t if_index,
    const char *types,
    const char *domain,
    cups_dnssd_browse_cb_t browse_cb,
    void *cb_data)
{
  (void)dnssd;
  (void)if_index;
  (void)types;
  (void)domain;
  (void)browse_cb;
  (void)cb_data;

  return (NULL);
}

int
cupsDNSSDDecodeTXT(
    const unsigned char *txtrec,
    uint16_t            txtlen,
    cups_option_t       **txt)
{
  int		num_txt = 0;
  unsigned char keylen;
  char		key[256],
		*value;
  const unsigned char *txtptr,
		*txtend;

  if (txt)
    *txt = NULL;
  if (!txtrec || !txtlen || !txt)
    return (0);

  for (txtptr = txtrec, txtend = txtrec + txtlen; txtptr < txtend; txtptr += keylen)
  {
    keylen = *txtptr++;
    if (keylen == 0 || (txtptr + keylen) > txtend)
      break;

    memcpy(key, txtptr, keylen);
    key[keylen] = '\0';

    if ((value = strchr(key, '=')) != NULL)
    {
      *value++ = '\0';
      num_txt = cupsAddOption(key, value, num_txt, txt);
    }
    else
    {
      break;
    }
  }

  return (num_txt);
}

void
cupsDNSSDDelete(cups_dnssd_t *dnssd)
{
  if (!dnssd)
    return;

  cupsRWDestroy(&dnssd->rwlock);
  free(dnssd);
}

char *
cupsDNSSDCopyComputerName(
    cups_dnssd_t *dnssd,
    char *buffer,
    size_t bufsize)
{
  (void)dnssd;

  if (buffer && bufsize > 0)
    *buffer = '\0';

  return (NULL);
}

char *
cupsDNSSDCopyHostName(
    cups_dnssd_t *dnssd,
    char *buffer,
    size_t bufsize)
{
  (void)dnssd;

  if (buffer && bufsize > 0)
    *buffer = '\0';

  return (NULL);
}

size_t
cupsDNSSDGetConfigChanges(cups_dnssd_t *dnssd)
{
  return (dnssd ? dnssd->config_changes : 0);
}

cups_dnssd_t *
cupsDNSSDNew(cups_dnssd_error_cb_t error_cb, void *cb_data)
{
  cups_dnssd_t *dnssd = calloc(1, sizeof(*dnssd));

  if (!dnssd)
    return (NULL);

  cupsRWInit(&dnssd->rwlock);
  dnssd->cb = error_cb;
  dnssd->cb_data = cb_data;

  return (dnssd);
}

void
cupsDNSSDQueryDelete(cups_dnssd_query_t *query)
{
  if (!query)
    return;

  free(query);
}

cups_dnssd_t *
cupsDNSSDQueryGetContext(cups_dnssd_query_t *query)
{
  return (query ? query->dnssd : NULL);
}

cups_dnssd_query_t *
cupsDNSSDQueryNew(
    cups_dnssd_t *dnssd,
    uint32_t if_index,
    const char *fullname,
    uint16_t rrtype,
    cups_dnssd_query_cb_t query_cb,
    void *cb_data)
{
  (void)dnssd;
  (void)if_index;
  (void)fullname;
  (void)rrtype;
  (void)query_cb;
  (void)cb_data;

  return (NULL);
}

void
cupsDNSSDResolveDelete(cups_dnssd_resolve_t *res)
{
  if (!res)
    return;

  free(res);
}

cups_dnssd_t *
cupsDNSSDResolveGetContext(cups_dnssd_resolve_t *res)
{
  return (res ? res->dnssd : NULL);
}

cups_dnssd_resolve_t *
cupsDNSSDResolveNew(
    cups_dnssd_t *dnssd,
    uint32_t if_index,
    const char *name,
    const char *type,
    const char *domain,
    cups_dnssd_resolve_cb_t resolve_cb,
    void *cb_data)
{
  (void)dnssd;
  (void)if_index;
  (void)name;
  (void)type;
  (void)domain;
  (void)resolve_cb;
  (void)cb_data;

  return (NULL);
}

bool
cupsDNSSDSeparateFullName(
    const char *fullname,
    char       *name,
    size_t     namesize,
    char       *type,
    size_t     typesize,
    char       *domain,
    size_t     domainsize)
{
  bool	ret = true;
  char	*ptr,
	*end;

  if (!fullname || !name || !namesize || !type || !typesize || !domain || !domainsize)
  {
    if (name)
      *name = '\0';
    if (type)
      *type = '\0';
    if (domain)
      *domain = '\0';

    return (false);
  }

  for (ptr = name, end = name + namesize - 1; *fullname; fullname++)
  {
    if (*fullname == '.')
      break;
    else if (*fullname == '\\' && isdigit(fullname[1] & 255) && isdigit(fullname[2] & 255) && isdigit(fullname[3] & 255))
    {
      if (ptr < end)
        *ptr++ = (char)((fullname[1] - '0') * 100 + (fullname[2] - '0') * 10 + fullname[3] - '0');
      else
        ret = false;

      fullname += 3;
    }
    else if (ptr < end)
      *ptr++ = *fullname;
    else
      ret = false;
  }
  *ptr = '\0';

  if (*fullname)
    fullname++;

  for (ptr = type, end = type + typesize - 1; *fullname; fullname++)
  {
    if (*fullname == '.' && fullname[1] != '_')
      break;
    else if (*fullname == '\\' && isdigit(fullname[1] & 255) && isdigit(fullname[2] & 255) && isdigit(fullname[3] & 255))
    {
      if (ptr < end)
        *ptr++ = (char)((fullname[1] - '0') * 100 + (fullname[2] - '0') * 10 + fullname[3] - '0');
      else
        ret = false;

      fullname += 3;
    }
    else if (ptr < end)
      *ptr++ = *fullname;
    else
      ret = false;
  }
  *ptr = '\0';

  if (*fullname)
    fullname++;

  for (ptr = domain, end = domain + domainsize - 1; *fullname; fullname++)
  {
    if (*fullname == '\\' && isdigit(fullname[1] & 255) && isdigit(fullname[2] & 255) && isdigit(fullname[3] & 255))
    {
      if (ptr < end)
        *ptr++ = (char)((fullname[1] - '0') * 100 + (fullname[2] - '0') * 10 + fullname[3] - '0');
      else
        ret = false;

      fullname += 3;
    }
    else if (ptr < end)
      *ptr++ = *fullname;
    else
      ret = false;
  }
  *ptr = '\0';

  return (ret);
}

bool
cupsDNSSDServiceAdd(
    cups_dnssd_service_t *service,
    const char *types,
    const char *domain,
    const char *host,
    uint16_t port,
    int num_txt,
    cups_option_t *txt)
{
  (void)service;
  (void)types;
  (void)domain;
  (void)host;
  (void)port;
  (void)num_txt;
  (void)txt;

  return (false);
}

void
cupsDNSSDServiceDelete(cups_dnssd_service_t *service)
{
  if (!service)
    return;

  if (service->name)
    free(service->name);

  free(service);
}

cups_dnssd_t *
cupsDNSSDServiceGetContext(cups_dnssd_service_t *service)
{
  return (service ? service->dnssd : NULL);
}

const char *
cupsDNSSDServiceGetName(cups_dnssd_service_t *service)
{
  return (service ? service->name : NULL);
}

cups_dnssd_service_t *
cupsDNSSDServiceNew(
    cups_dnssd_t *dnssd,
    uint32_t if_index,
    const char *name,
    cups_dnssd_service_cb_t cb,
    void *cb_data)
{
  cups_dnssd_service_t *service;

  (void)if_index;
  (void)cb;
  (void)cb_data;

  if (!dnssd || !name)
    return (NULL);

  if ((service = calloc(1, sizeof(*service))) == NULL)
    return (NULL);

  service->dnssd = dnssd;
  service->name = strdup(name);

  if (!service->name)
  {
    free(service);
    return (NULL);
  }

  return (service);
}

bool
cupsDNSSDServicePublish(cups_dnssd_service_t *service)
{
  (void)service;

  return (false);
}

bool
cupsDNSSDServiceSetLocation(
    cups_dnssd_service_t *service,
    const char *geo_uri)
{
  (void)service;
  (void)geo_uri;

  return (false);
}
