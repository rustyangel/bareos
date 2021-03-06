/*
   BAREOS® - Backup Archiving REcovery Open Sourced

   Copyright (C) 2000-2010 Free Software Foundation Europe e.V.
   Copyright (C) 2011-2012 Planets Communications B.V.
   Copyright (C) 2013-2013 Bareos GmbH & Co. KG

   This program is Free Software; you can redistribute it and/or
   modify it under the terms of version three of the GNU Affero General Public
   License as published by the Free Software Foundation and included
   in the file LICENSE.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/
/*
 * Kern Sibbald, January MM
 */

struct RES_ITEM;                        /* Declare forward referenced structure */
class RES;                              /* Declare forware referenced structure */

/*
 * Parser state
 */
enum parse_state {
   p_none,
   p_resource
};

/*
 * Password encodings.
 */
enum password_encoding {
   p_encoding_clear,
   p_encoding_md5
};

/*
 * Used for message destinations.
 */
struct s_mdestination {
   int code;
   const char *destination;
   bool where;
};

/*
 * Used for message types.
 */
struct s_mtypes {
   const char *name;
   int token;
};

/*
 * Used for certain KeyWord tables
 */
struct s_kw {
   const char *name;
   unsigned int token;
};

/*
 * Used to store passwords with their encoding.
 */
struct s_password {
   enum password_encoding encoding;
   char *value;
};

/*
 * This is the structure that defines the record types (items) permitted within each
 * resource. It is used to define the configuration tables.
 */
struct RES_ITEM {
   const char *name;                    /* Resource name i.e. Director, ... */
   const int type;                      /* Type of resource see CFG_TYPE_* */
   union {
      char **value;                     /* Where to store the item */
      uint32_t *ui32value;
      int32_t *i32value;
      uint64_t *ui64value;
      int64_t *i64value;
      bool *boolvalue;
      utime_t *utimevalue;
      s_password *pwdvalue;
      RES **resvalue;
      alist **alistvalue;
      dlist **dlistvalue;
   };
   int32_t code;                        /* Item code/additional info */
   uint32_t flags;                      /* Flags: See CFG_ITEM_* */
   const char *default_value;           /* Default value */
};

/*
 * For storing name_addr items in res_items table
 */
#define ITEM(x) {(char **)&res_all.x}

#define MAX_RES_ITEMS 80                /* Maximum resource items per RES */

/*
 * This is the universal header that is at the beginning of every resource record.
 */
class RES {
public:
   rblink link;                         /* Red-black list link */
   RES *res_next;                       /* Pointer to next resource of this type */
   char *name;                          /* Resource name */
   char *desc;                          /* Resource description */
   uint32_t rcode;                      /* Resource id or type */
   int32_t refcnt;                      /* Reference count for releasing */
   char item_present[MAX_RES_ITEMS];    /* Set if item is present in conf file */
};

/*
 * Master Resource configuration structure definition
 * This is the structure that defines the
 * resources that are available to this daemon.
 */
struct RES_TABLE {
   const char *name;                    /* Resource name */
   RES_ITEM *items;                     /* List of resource keywords */
   uint32_t rcode;                      /* Code if needed */
   uint32_t size;                       /* Size of resource */
};

/*
 * The RES_CONTAINER is a container for all resources of a certain type normally
 * any daemon has multiple containers each holding a certain type of resources.
 * When a daemon supports 4 different resource types it has 4 containers of which
 * some may be empty (e.g. head == tail == NULL) when there are no such resources
 * in the config defined.
 *
 * We use a red-black list for storing the resources which is both fast and flexible.
 * As the rblist implementation doesn't have a remove method and the destroy method
 * also says its not working we keep track of the head and tail of the entries on the
 * list so on a destroy of all items (e.g. on reload) we can easily walk this list for
 * destroying all items on the list and don't have to do an indepth walk of the
 * red-black tree.
 */
class RES_CONTAINER {
public:
   rblist *list;                        /* Resource list */
   RES *head;                           /* Head of the list */
   RES *tail;                           /* Tail of the list */
};

/* Common Resource definitions */

#define MAX_RES_NAME_LENGTH (MAX_NAME_LENGTH - 1)  /* maximum resource name length */

/*
 * Item specific flags.
 */
#define CFG_ITEM_REQUIRED          0x1  /* Item required */
#define CFG_ITEM_DEFAULT           0x2  /* Default supplied */
#define CFG_ITEM_NO_EQUALS         0x4  /* Don't scan = after name */
#define CFG_ITEM_DEPRECATED        0x8  /* Deprecated config option */
#define CFG_ITEM_ALIAS             0x10 /* Alias for other keyword */

/*
 * Standard global types with handlers defined in res.c
 */
#define CFG_TYPE_STR                1   /* String */
#define CFG_TYPE_DIR                2   /* Directory */
#define CFG_TYPE_MD5PASSWORD        3   /* MD5 hashed Password */
#define CFG_TYPE_CLEARPASSWORD      4   /* Clear text Password */
#define CFG_TYPE_NAME               5   /* Name */
#define CFG_TYPE_STRNAME            6   /* String Name */
#define CFG_TYPE_RES                7   /* Resource */
#define CFG_TYPE_ALIST_RES          8   /* List of resources */
#define CFG_TYPE_ALIST_STR          9   /* List of strings */
#define CFG_TYPE_ALIST_DIR          10  /* List of dirs */
#define CFG_TYPE_INT32              11  /* 32 bits Integer */
#define CFG_TYPE_PINT32             12  /* Positive 32 bits Integer (unsigned) */
#define CFG_TYPE_MSGS               13  /* Message resource */
#define CFG_TYPE_INT64              14  /* 64 bits Integer */
#define CFG_TYPE_BIT                15  /* Bitfield */
#define CFG_TYPE_BOOL               16  /* Boolean */
#define CFG_TYPE_TIME               17  /* Time value */
#define CFG_TYPE_SIZE64             18  /* 64 bits file size */
#define CFG_TYPE_SIZE32             19  /* 32 bits file size */
#define CFG_TYPE_SPEED              20  /* Speed limit */
#define CFG_TYPE_DEFS               21  /* Definition */
#define CFG_TYPE_LABEL              22  /* Label */
#define CFG_TYPE_ADDRESSES          23  /* List of ip addresses */
#define CFG_TYPE_ADDRESSES_ADDRESS  24  /* Ip address */
#define CFG_TYPE_ADDRESSES_PORT     25  /* Ip port */

/*
 * Base Class for all Resource Classes
 */
class BRSRES {
   public:
      RES hdr;

   /* Methods */
   char *name() const;
   bool print_config(POOL_MEM &buf);
};

inline char *BRSRES::name() const { return this->hdr.name; }

/*
 * Message Resource
 */
class MSGSRES : public BRSRES {
   /*
    * Members
    */
public:
   char *mail_cmd;                    /* mail command */
   char *operator_cmd;                /* Operator command */
   DEST *dest_chain;                  /* chain of destinations */
   char send_msg[nbytes_for_bits(M_MAX+1)];  /* bit array of types */

private:
   bool m_in_use;                     /* set when using to send a message */
   bool m_closing;                    /* set when closing message resource */

public:
   /*
    * Methods
    */
//   char *name() const;
   void clear_in_use() { lock(); m_in_use=false; unlock(); }
   void set_in_use() { wait_not_in_use(); m_in_use=true; unlock(); }
   void set_closing() { m_closing=true; }
   bool get_closing() { return m_closing; }
   void clear_closing() { lock(); m_closing=false; unlock(); }
   bool is_closing() { lock(); bool rtn=m_closing; unlock(); return rtn; }

   void wait_not_in_use();            /* in message.c */
   void lock();                       /* in message.c */
   void unlock();                     /* in message.c */
   bool print_config(POOL_MEM& buff);
};

class CONFIG;
typedef void (INIT_RES_HANDLER)(RES_ITEM *item);
typedef void (STORE_RES_HANDLER)(CONFIG *config, LEX *lc, RES_ITEM *item, int index, int pass);

/*
 * Configfile class
 */
class CONFIG {
public:
   /*
    * Members
    */
   const char *m_cf;                    /* Config file being processed */
   LEX_ERROR_HANDLER *m_scan_error;     /* Error handler if non-null */
   LEX_WARNING_HANDLER *m_scan_warning; /* Warning handler if non-null */
   INIT_RES_HANDLER *m_init_res;        /* Init resource handler for non default types if non-null */
   STORE_RES_HANDLER *m_store_res;      /* Store resource handler for non default types if non-null */

   int32_t m_err_type;                  /* The way to terminate on failure */
   void *m_res_all;                     /* Pointer to res_all buffer */
   int32_t m_res_all_size;              /* Length of buffer */

   int32_t m_r_first;                   /* First daemon resource type */
   int32_t m_r_last;                    /* Last daemon resource type */
   RES_TABLE *m_resources;              /* Pointer to array of pointers to the different tables with permitted resources */
   RES_CONTAINER **m_res_containers;    /* Pointer to array of pointers to the different containers with resources per type */
   brwlock_t m_res_lock;                /* Resource lock */

   /*
    * Methods
    */
   void init(const char *cf,
             LEX_ERROR_HANDLER *scan_error,
             LEX_WARNING_HANDLER *scan_warning,
             INIT_RES_HANDLER *init_res,
             STORE_RES_HANDLER *store_res,
             int32_t err_type,
             void *vres_all,
             int32_t res_all_size,
             int32_t r_first,
             int32_t r_last,
             RES_TABLE *resources);
   bool parse_config();
   bool store_resource(int type, LEX *lc, RES_ITEM *item, int index, int pass);
   void initialize_resource(int type, RES_ITEM *items, int pass);
   void insert_resource(int rindex, RES *res, bool remove_duplicate);
   void insert_resource(int rindex, int size);
   void insert_resource(int rindex, int size, LEX *lc);
   void dump_all_resources(int type, void sendmsg(void *sock, const char *fmt, ...), void *sock);
   void new_res_containers();
   void free_all_resources();
   const char *res_to_str(int rcode);
   RES *GetResWithName(int rcode, const char *name);
   RES *GetNextRes(int rcode, RES *res);
   void b_LockRes(const char *file, int line);
   void b_UnlockRes(const char *file, int line);
};

CONFIG *new_config_parser();

/*
 * Resource routines
 */
void free_resource(RES *res, int type);
void save_resource(CONFIG *config, int type, RES_ITEM *item, int pass);
void dump_resource(CONFIG *config, int type, RES *res, void sendmsg(void *sock, const char *fmt, ...), void *sock);

#define LockRes(config) (config)->b_LockRes(__FILE__, __LINE__)
#define UnlockRes(config) (config)->b_UnlockRes(__FILE__, __LINE__)

/*
 * Loop through each resource of type, returning in var
 */
#ifdef HAVE_TYPEOF
#define foreach_res(config, var, type) \
        for((var)=NULL; ((var)=(typeof(var))(config)->GetNextRes((type), (RES *)var));)
#else
#define foreach_res(config, var, type) \
    for(var=NULL; (*((void **)&(var))=(void *)(config)->GetNextRes((type), (RES *)var));)
#endif
