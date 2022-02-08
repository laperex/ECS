#pragma once

#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

class Registry {
private:
	Registry() {}
	EntityManager *entity_manager = EntityManager::sharedInstance();
	ComponentManager *component_manager = ComponentManager::sharedInstance();
	SystemManager *system_manager = SystemManager::sharedInstance();

public:
	static Registry *sharedInstance() {
		static Registry instance;
		return &instance;
	}

	Entity CreateEntity() {
		return entity_manager->CreateEntity();
	}

	template <typename T>
	Entity CreateEntity() {
		
	}

	template <typename T>
	void Register() {
		component_manager->Register<T>();
	}

	template <typename T>
	void AddComponent(const Entity &entity, T data) {
		entity_manager->AddComponent<T>(entity);
		component_manager->AddComponent<T>(entity, data);
		system_manager->EntityIDChanged<T>(entity);
	}

	template <typename T>
	void RemoveComponent(const Entity &entity) {
		entity_manager->RemoveComponent<T>(entity);
		component_manager->RemoveComponent<T>(entity);
		system_manager->EntityIDChanged<T>(entity);
	}

	template <typename T>
	T& GetComponent(const Entity &entity) {
		return component_manager->GetComponent<T>(entity);
	}

	void DestroyEntity(const Entity &entity) {
		entity_manager->DestroyEntity(entity);
		component_manager->DestroyEntity(entity);
	}

	template <typename ...T>
	std::set<Entity>& View() {
		system_manager->CreateView<T...>();
	}
};