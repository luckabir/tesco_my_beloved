#ifndef CUSTOMER_MANAGER_H
#define CUSTOMER_MANAGER_H

#include "../classes/Customer.h"
#include "AssetManager.h"
#include <memory>

class CustomerManager {
public:
    static std::shared_ptr<Customer> CreateCustomer();
    static ItemTemplate PickItemForCustomer(const Customer& customer);
    static QTEEvent PickQTEForCustomer(const Customer& customer);

private:
    static std::shared_ptr<Customer> CreateRandomCustomer();
    static std::shared_ptr<Customer> CreateRegularCustomer();
};

#endif