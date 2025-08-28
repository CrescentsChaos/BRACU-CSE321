#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 5
#define MAX_RESOURCES 5
#define MAX_NAME_LEN 20

typedef enum {
    READ = 1,
    WRITE = 2,
    EXECUTE = 4
} Permission;

typedef struct {
    char name[MAX_NAME_LEN];
} User;

typedef struct {
    char name[MAX_NAME_LEN];
} Resource;

typedef struct {
    char username[MAX_NAME_LEN];
    int permissions;
} ACLEntry;

typedef struct {
    Resource resource;
    ACLEntry acl_entries[MAX_USERS];
    int acl_count;
} ACLControlledResource;

typedef struct {
    char resource_name[MAX_NAME_LEN];
    int permissions;
} Capability;

typedef struct {
    User user;
    Capability capabilities[MAX_RESOURCES];
    int capability_count;
} CapabilityUser;

void printPermissions(int perm) {
    if (perm == 0) {
        printf("None");
        return;
    }
    int first = 1;
    if (perm & READ) {
        printf("Read");
        first = 0;
    }
    if (perm & WRITE) {
        if (!first) printf(", ");
        printf("Write");
        first = 0;
    }
    if (perm & EXECUTE) {
        if (!first) printf(", ");
        printf("Execute");
    }
}

int hasPermission(int userPerm, int requiredPerm) {
    return (userPerm & requiredPerm) == requiredPerm;
}

void checkACLAccess(ACLControlledResource *res, const char *userName, int perm) {
    for (int i = 0; i < res->acl_count; i++) {
        if (strcmp(res->acl_entries[i].username, userName) == 0) {
            if (hasPermission(res->acl_entries[i].permissions, perm)) {
                printf("ACL Check: User %s requests ", userName);
                printPermissions(perm);
                printf(" on %s: Access GRANTED\n", res->resource.name);
            } else {
                printf("ACL Check: User %s requests ", userName);
                printPermissions(perm);
                printf(" on %s: Access DENIED\n", res->resource.name);
            }
            return;
        }
    }
    printf("ACL Check: User %s has NO entry for resource %s: Access DENIED\n", 
           userName, res->resource.name);
}

void checkCapabilityAccess(CapabilityUser *user, const char *resourceName, int perm) {
    for (int i = 0; i < user->capability_count; i++) {
        if (strcmp(user->capabilities[i].resource_name, resourceName) == 0) {
            if (hasPermission(user->capabilities[i].permissions, perm)) {
                printf("Capability Check: User %s requests ", user->user.name);
                printPermissions(perm);
                printf(" on %s: Access GRANTED\n", resourceName);
            } else {
                printf("Capability Check: User %s requests ", user->user.name);
                printPermissions(perm);
                printf(" on %s: Access DENIED\n", resourceName);
            }
            return;
        }
    }
    printf("Capability Check: User %s has NO capability for %s: Access DENIED\n", 
           user->user.name, resourceName);
}

void addACLEntry(ACLControlledResource *res, const char *username, int permissions) {
    if (res->acl_count < MAX_USERS) {
        strcpy(res->acl_entries[res->acl_count].username, username);
        res->acl_entries[res->acl_count].permissions = permissions;
        res->acl_count++;
    }
}

void addCapability(CapabilityUser *user, const char *resourceName, int permissions) {
    if (user->capability_count < MAX_RESOURCES) {
        strcpy(user->capabilities[user->capability_count].resource_name, resourceName);
        user->capabilities[user->capability_count].permissions = permissions;
        user->capability_count++;
    }
}

int main() {
    User users[MAX_USERS] = {
        {"Alice"}, {"Bob"}, {"Charlie"}, {"David"}, {"Eve"}
    };
    Resource resources[MAX_RESOURCES] = {
        {"File1"}, {"File2"}, {"File3"}, {"Database"}, {"Printer"}
    };

    ACLControlledResource acl_resources[MAX_RESOURCES];
    CapabilityUser cap_users[MAX_USERS];

    // Initialize ACL resources
    for (int i = 0; i < MAX_RESOURCES; i++) {
        acl_resources[i].resource = resources[i];
        acl_resources[i].acl_count = 0;
    }

    // Initialize capability users
    for (int i = 0; i < MAX_USERS; i++) {
        cap_users[i].user = users[i];
        cap_users[i].capability_count = 0;
    }

    // ACL Setup
    addACLEntry(&acl_resources[0], "Alice", READ | WRITE);
    addACLEntry(&acl_resources[0], "Bob", READ);
    addACLEntry(&acl_resources[1], "Alice", READ | EXECUTE);
    addACLEntry(&acl_resources[1], "Charlie", WRITE);
    addACLEntry(&acl_resources[2], "Bob", READ | WRITE | EXECUTE);
    addACLEntry(&acl_resources[3], "David", READ | WRITE);
    addACLEntry(&acl_resources[4], "Eve", READ);

    // Capability Setup
    addCapability(&cap_users[0], "File1", READ | WRITE);
    addCapability(&cap_users[0], "File2", READ | EXECUTE);
    addCapability(&cap_users[1], "File1", READ);
    addCapability(&cap_users[1], "File3", READ | WRITE | EXECUTE);
    addCapability(&cap_users[2], "File2", WRITE);
    addCapability(&cap_users[3], "Database", READ | WRITE);
    addCapability(&cap_users[4], "Database", READ);

    // Test ACL
    checkACLAccess(&acl_resources[0], "Alice", READ);
    checkACLAccess(&acl_resources[0], "Bob", WRITE);
    checkACLAccess(&acl_resources[1], "Charlie", EXECUTE);
    checkACLAccess(&acl_resources[3], "David", WRITE);
    checkACLAccess(&acl_resources[4], "Eve", READ);
    checkACLAccess(&acl_resources[4], "Alice", READ);

    // Test Capability
    checkCapabilityAccess(&cap_users[0], "File1", WRITE);
    checkCapabilityAccess(&cap_users[1], "File3", EXECUTE);
    checkCapabilityAccess(&cap_users[2], "File2", WRITE);
    checkCapabilityAccess(&cap_users[3], "Database", READ);
    checkCapabilityAccess(&cap_users[4], "Database", WRITE);
    checkCapabilityAccess(&cap_users[0], "Printer", READ);

    return 0;
}