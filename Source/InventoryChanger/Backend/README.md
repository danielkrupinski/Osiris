# <sub><sup>Inventory Changer /</sup></sub> Backend

## What is Backend

The Backend is a component of the Inventory Changer which implements the functionality of CSGO's item server for items created by the Inventory Changer.
The main class which is used by other Inventory Changer components to interact with the backend is [BackendSimulator](BackendSimulator.h).

## What is a Request

To ensure integrity and synchronization between the backend and CSGO's inventory, item properties are **not modifiable** from outside the backend.
In order to modify a item, a backend request is needed. Backend request-related code is located in [Request](Request/) subdirectory. Requests are handled immediately.

## What is a Response

A response is the way to tell CSGO's inventory to update in response to changes in the backend. Backend response-related code is located in [Response](Response/) subdirectory. Responses are queued (to simulate the delay of the real item server) and dispatched by `BackendSimulator::run()` method.
