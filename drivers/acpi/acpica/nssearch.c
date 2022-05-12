// SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0
/*******************************************************************************
 *
 * Module Name: nssearch - Namespace search
 *
 ******************************************************************************/

#include <acpi/acpi.h>
#include "accommon.h"
#include "acnamesp.h"

#ifdef ACPI_ASL_COMPILER
#include "amlcode.h"
#endif

#define _COMPONENT          ACPI_NAMESPACE
ACPI_MODULE_NAME("nssearch")

/* Local prototypes */
static acpi_status
acpi_ns_search_parent_tree(u32 target_name,
			   struct acpi_namespace_node *node,
			   acpi_object_type type,
			   struct acpi_namespace_node **return_node);

/*******************************************************************************
 *
 * FUNCTION:    acpi_ns_search_one_scope
 *
 * PARAMETERS:  target_name     - Ascii ACPI name to search for
 *              parent_node     - Starting node where search will begin
 *              type            - Object type to match
 *              return_node     - Where the matched Named obj is returned
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Search a single level of the namespace. Performs a
 *              simple search of the specified level, and does not add
 *              entries or search parents.
 *
 *
 *      Named object lists are built (and subsequently dumped) in the
 *      order in which the names are encountered during the namespace load;
 *
 *      All namespace searching is linear in this implementation, but
 *      could be easily modified to support any improved search
 *      algorithm. However, the linear search was chosen for simplicity
 *      and because the trees are small and the other interpreter
 *      execution overhead is relatively high.
 *
 *      Note: CPU execution analysis has shown that the AML interpreter spends
 *      a very small percentage of its time searching the namespace. Therefore,
 *      the linear search seems to be sufficient, as there would seem to be
 *      little value in improving the search.
 *
 ******************************************************************************/

acpi_status
acpi_ns_search_one_scope(u32 target_name,
			 struct acpi_namespace_node *parent_node,
			 acpi_object_type type,
			 struct acpi_namespace_node **return_node)
{
	struct acpi_namespace_node *node;

	ACPI_FUNCTION_TRACE(ns_search_one_scope);

#ifdef ACPI_DEBUG_OUTPUT
	if (ACPI_LV_NAMES & acpi_dbg_level) {
		char *scope_name;

		scope_name = acpi_ns_get_normalized_pathname(parent_node, TRUE);
		if (scope_name) {
			ACPI_DEBUG_PRINT((ACPI_DB_NAMES,
					  "Searching %s (%p) For [%4.4s] (%s)\n",
					  scope_name, parent_node,
					  ACPI_CAST_PTR(char, &target_name),
					  acpi_ut_get_type_name(type)));

			ACPI_FREE(scope_name);
		}
	}
#endif

	/*
	 * Search for name at this namespace level, which is to say that we
	 * must search for the name among the children of this object
	 */
	node = parent_node->child;
	while (node) {

		/* Check for match against the name */

		if (node->name.integer == target_name) {

			/* Resolve a control method alias if any */

			if (acpi_ns_get_type(node) ==
			    ACPI_TYPE_LOCAL_METHOD_ALIAS) {
				node =
				    ACPI_CAST_PTR(struct acpi_namespace_node,
						  node->object);
			}

			/* Found matching entry */

			ACPI_DEBUG_PRINT((ACPI_DB_NAMES,
					  "Name [%4.4s] (%s) %p found in scope [%4.4s] %p\n",
					  ACPI_CAST_PTR(char, &target_name),
					  acpi_ut_get_type_name(node->type),
					  node,
					  acpi_ut_get_node_name(parent_node),
					  parent_node));

			*return_node = node;
			return_ACPI_STATUS(AE_OK);
		}

		/* Didn't match name, move on to the next peer object */

		node = node->peer;
	}

	/* Searched entire namespace level, not found */

	ACPI_DEBUG_PRINT((ACPI_DB_NAMES,
			  "Name [%4.4s] (%s) not found in search in scope [%4.4s] "
			  "%p first child %p\n",
			  ACPI_CAST_PTR(char, &target_name),
			  acpi_ut_get_type_name(type),
			  acpi_ut_get_node_name(parent_node), parent_node,
			  parent_node->child));

	return_ACPI_STATUS(AE_NOT_FOUND);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ns_search_parent_tree
 *
 * PARAMETERS:  target_name     - Ascii ACPI name to search for
 *              node            - Starting node where search will begin
 *              type            - Object type to match
 *              return_node     - Where the matched Node is returned
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Called when a name has not been found in the current namespace
 *              level. Before adding it or giving up, ACPI scope rules require
 *              searching enclosing scopes in cases identified by acpi_ns_local().
 *
 *              "A name is located by finding the matching name in the current
 *              name space, and then in the parent name space. If the parent
 *              name space does not contain the name, the search continues
 *              recursively until either the name is found or the name space
 *              does not have a parent (the root of the name space). This
 *              indicates that the name is not found" (From ACPI Specification,
 *              section 5.3)
 *
 ******************************************************************************/

static acpi_status
acpi_ns_search_parent_tree(u32 target_name,
			   struct acpi_namespace_node *node,
			   acpi_object_type type,
			   struct acpi_namespace_node **return_node)
{
	acpi_status status;
	struct acpi_namespace_node *parent_node;

	ACPI_FUNCTION_TRACE(ns_search_parent_tree);

	parent_node = node->parent;

	/*
	 * If there is no parent (i.e., we are at the root) or type is "local",
	 * we won't be searching the parent tree.
	 */
	if (!parent_node) {
		ACPI_DEBUG_PRINT((ACPI_DB_NAMES, "[%4.4s] has no parent\n",
				  ACPI_CAST_PTR(char, &target_name)));
		return_ACPI_STATUS(AE_NOT_FOUND);
	}

	if (acpi_ns_local(type)) {
		ACPI_DEBUG_PRINT((ACPI_DB_NAMES,
				  "[%4.4s] type [%s] must be local to this scope (no parent search)\n",
				  ACPI_CAST_PTR(char, &target_name),
				  acpi_ut_get_type_name(type)));
		return_ACPI_STATUS(AE_NOT_FOUND);
	}

	/* Search the parent tree */

	ACPI_DEBUG_PRINT((ACPI_DB_NAMES,
			  "Searching parent [%4.4s] for [%4.4s]\n",
			  acpi_ut_get_node_name(parent_node),
			  ACPI_CAST_PTR(char, &target_name)));

	/* Search parents until target is found or we have backed up to the root */

	while (parent_node) {
		/*
		 * Search parent scope. Use TYPE_ANY because we don't care about the
		 * object type at this point, we only care about the existence of
		 * the actual name we are searching for. Typechecking comes later.
		 */
		status =
		    acpi_ns_search_one_scope(target_name, parent_node,
					     ACPI_TYPE_ANY, return_node);
		if (ACPI_SUCCESS(status)) {
			return_ACPI_STATUS(status);
		}

		/* Not found here, go up another level (until we reach the root) */

		parent_node = parent_node->parent;
	}

	/* Not found in parent tree */

	return_ACPI_STATUS(AE_NOT_FOUND);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ns_search_and_enter
 *
 * PARAMETERS:  target_name         - Ascii ACPI name to search for (4 chars)
 *              walk_state          - Current state of the walk
 *              node                - Starting node where search will begin
 *              interpreter_mode    - Add names only in ACPI_MODE_LOAD_PASS_x.
 *                                    Otherwise,search only.
 *              type                - Object type to match
 *              flags               - Flags describing the search restrictions
 *              return_node         - Where the Node is returned
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Search for a name segment in a single namespace level,
 *              optionally adding it if it is not found. If the passed
 *              Type is not Any and the type previously stored in the
 *              entry was Any (i.e. unknown), update the stored type.
 *
 *              In ACPI_IMODE_EXECUTE, search only.
 *              In other modes, search and add if not found.
 *
 ******************************************************************************/

acpi_status
acpi_ns_search_and_enter(u32 target_name,
			 struct acpi_walk_state *walk_state,
			 struct acpi_namespace_node *node,
			 acpi_interpreter_mode interpreter_mode,
			 acpi_object_type type,
			 u32 flags, struct acpi_namespace_node **return_node)
{
	acpi_status status;
	struct acpi_namespace_node *new_node;

	ACPI_FUNCTION_TRACE(ns_search_and_enter);

	/* Parameter validation */

	if (!node || !target_name || !return_node) {
		ACPI_ERROR((AE_INFO,
			    "Null parameter: Node %p Name 0x%X ReturnNode %p",
			    node, target_name, return_node));
		return_ACPI_STATUS(AE_BAD_PARAMETER);
	}

	/*
	 * Name must consist of valid ACPI characters. We will repair the name if
	 * necessary because we don't want to abort because of this, but we want
	 * all namespace names to be printable. A warning message is appropriate.
	 *
	 * This issue came up because there are in fact machines that exhibit
	 * this problem, and we want to be able to enable ACPI support for them,
	 * even though there are a few bad names.
	 */
	acpi_ut_repair_name(ACPI_CAST_PTR(char, &target_name));

	/* Try to find the name in the namespace level specified by the caller */

	*return_node = ACPI_ENTRY_NOT_FOUND;
	status = acpi_ns_search_one_scope(target_name, node, type, return_node);
	if (status != AE_NOT_FOUND) {
		/*
		 * If we found it AND the request specifies that a find is an error,
		 * return the error
		 */
		if (status == AE_OK) {

			/* The node was found in the namespace */

			/*
			 * If the namespace override feature is enabled for this node,
			 * delete any existing attached sub-object and make the node
			 * look like a new node that is owned by the override table.
			 */
			if (flags & ACPI_NS_OVERRIDE_IF_FOUND) {
				ACPI_DEBUG_PRINT((ACPI_DB_NAMES,
						  "Namespace override: %4.4s pass %u type %X Owner %X\n",
						  ACPI_CAST_PTR(char,
								&target_name),
						  interpreter_mode,
						  (*return_node)->type,
						  walk_state->owner_id));

				acpi_ns_delete_children(*return_node);
				if (acpi_gbl_runtime_namespace_override) {
					acpi_ut_remove_reference((*return_node)->object);
					(*return_node)->object = NULL;
					(*return_node)->owner_id =
					    walk_state->owner_id;
				} else {
					acpi_ns_remove_node(*return_node);
					*return_node = ACPI_ENTRY_NOT_FOUND;
				}
			}

			/* Return an error if we don't expect to find the object */

			else if (flags & ACPI_NS_ERROR_IF_FOUND) {
				status = AE_ALREADY_EXISTS;
			}
		}
#ifdef ACPI_ASL_COMPILER
		if (*return_node && (*return_node)->type == ACPI_TYPE_ANY) {
			(*return_node)->flags |= ANOBJ_IS_EXTERNAL;
		}
#endif

		/* Either found it or there was an error: finished either way */

		return_ACPI_STATUS(status);
	}

	/*
	 * The name was not found. If we are NOT performing the first pass
	 * (name entry) of loading the namespace, search the parent tree (all the
	 * way to the root if necessary.) We don't want to perform the parent
	 * search when the namespace is actually being loaded. We want to perform
	 * the search when namespace references are being resolved (load pass 2)
	 * and during the execution phase.
	 */
	if ((interpreter_mode != ACPI_IMODE_LOAD_PASS1) &&
	    (flags & ACPI_NS_SEARCH_PARENT)) {
		/*
		 * Not found at this level - search parent tree according to the
		 * ACPI specification
		 */
		status =
		    acpi_ns_search_parent_tree(target_name, node, type,
					       return_node);
		if (ACPI_SUCCESS(status)) {
			return_ACPI_STATUS(status);
		}
	}

	/* In execute mode, just search, never add names. Exit now */

	if (interpreter_mode == ACPI_IMODE_EXECUTE) {
		ACPI_DEBUG_PRINT((ACPI_DB_NAMES,
				  "%4.4s Not found in %p [Not adding]\n",
				  ACPI_CAST_PTR(char, &target_name), node));

		return_ACPI_STATUS(AE_NOT_FOUND);
	}

	/* Create the new named object */

	new_node = acpi_ns_create_node(target_name);
	if (!new_node) {
		return_ACPI_STATUS(AE_NO_MEMORY);
	}
#ifdef ACPI_ASL_COMPILER

	/* Node is an object defined by an External() statement */

	if (flags & ACPI_NS_EXTERNAL ||
	    (walk_state && walk_state->opcode == AML_SCOPE_OP)) {
		new_node->flags |= ANOBJ_IS_EXTERNAL;
	}
#endif

	if (flags & ACPI_NS_TEMPORARY) {
		new_node->flags |= ANOBJ_TEMPORARY;
	}

	/* Install the new object into the parent's list of children */

	acpi_ns_install_node(walk_state, node, new_node, type);
	*return_node = new_node;
	return_ACPI_STATUS(AE_OK);
}
