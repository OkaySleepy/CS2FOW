#include "test_suites.h"

#include "runtime_compatibility_model.h"
#include "runtime_health.h"
#include "settings_model.h"

#include <cassert>
#include <chrono>

using namespace cs2fow;

void run_structure_tests()
{
	using namespace std::chrono_literals;
	const auto start = std::chrono::steady_clock::time_point(1s);
	configuration_transaction transaction;
	assert(transaction.state() == configuration_load_state::defaults);
	assert(transaction.active() == runtime_configuration {});
	assert(transaction.begin(start));
	assert(!transaction.begin(start + 1ms));
	assert(!transaction.timed_out(start + 4999ms, 5s));
	assert(transaction.timed_out(start + 5s, 5s));
	configuration_transaction initial_failure;
	assert(initial_failure.begin(start));
	assert(initial_failure.rollback());
	assert(initial_failure.active() == runtime_configuration {});
	assert(initial_failure.state() == configuration_load_state::failed);

	runtime_configuration candidate;
	candidate.worker_threads = 4;
	candidate.visibility_hold_ms = 60;
	const uint32_t changes = transaction.commit(candidate, start + 5s);
	assert((changes & setting_change_worker_threads) != 0);
	assert((changes & setting_change_visibility) != 0);
	assert(transaction.active() == candidate);
	assert(transaction.state() == configuration_load_state::loaded);

	assert(transaction.begin(start + 6s));
	runtime_configuration broken = candidate;
	broken.enable = false;
	assert(transaction.rollback());
	assert(transaction.active() == candidate);
	assert(transaction.state() == configuration_load_state::failed);

	runtime_configuration direct = candidate;
	direct.debug = true;
	assert(transaction.apply_direct(direct) == setting_change_debug);
	assert(transaction.active() == direct);

	runtime_configuration invalid;
	invalid.enable = false;
	invalid.shoulder_base_units = 100.0f;
	invalid.max_shoulder_units = 50.0f;
	invalid.he_clear_radius_units = 0.0f;
	invalid.debug_los_player = 1;
	const uint32_t findings = validate_configuration(invalid);
	assert((findings & configuration_finding_disabled) != 0);
	assert((findings & configuration_finding_shoulder_range) != 0);
	assert((findings & configuration_finding_he_pair) != 0);
	assert((findings & configuration_finding_debug_los) != 0);

	const compatibility_report compatible = make_compatibility_report(
		compatibility_state::compatible, "verified", {"temporary LOS debug beams"});
	assert(compatible.operator_action.empty());
	assert(compatible.missing_capabilities.size() == 1);
	const compatibility_report update = make_compatibility_report(
		compatibility_state::update_required, "fingerprint mismatch");
	assert(update.operator_action.find("package") != std::string::npos);
	const compatibility_report unsupported = make_compatibility_report(
		compatibility_state::unsupported_system, "AVX missing");
	assert(unsupported.operator_action.find("AVX") != std::string::npos);
	const compatibility_report error = make_compatibility_report(
		compatibility_state::error, "schema unavailable");
	assert(error.operator_action.find("metrics") != std::string::npos);
	assert(std::string(runtime_health_state_name(
		runtime_health_state::loading_configuration)) == "LOADING CONFIGURATION");
	assert(std::string(runtime_health_state_name(
		runtime_health_state::protected_state)) == "PROTECTED");
	assert(std::string(runtime_health_state_name(runtime_health_state::starting)) == "STARTING");
	assert(std::string(runtime_health_state_name(runtime_health_state::loading_map)) == "LOADING MAP");
	assert(std::string(runtime_health_state_name(runtime_health_state::baking)) == "BAKING");
	assert(std::string(runtime_health_state_name(runtime_health_state::disabled)) == "DISABLED");
	assert(std::string(runtime_health_state_name(runtime_health_state::update_required)) == "UPDATE REQUIRED");
	assert(std::string(runtime_health_state_name(runtime_health_state::unsupported_system)) == "UNSUPPORTED SYSTEM");
	assert(std::string(runtime_health_state_name(runtime_health_state::error)) == "ERROR");
}
