<?php

namespace MediaWiki\Hook;

use Skin;

/**
 * This is a hook handler interface, see docs/Hooks.md.
 * Use the hook name "SkinGetPoweredBy" to register handlers implementing this interface.
 *
 * @deprecated since 1.37
 * @ingroup Hooks
 */
interface SkinGetPoweredByHook {
	/**
	 * This hook is called when generating the code used to display the
	 * "Powered by MediaWiki" icon.
	 *
	 * @since 1.35
	 *
	 * @param string &$text Additional 'powered by' icons in HTML.
	 *   Note: Modern skin does not use the MediaWiki icon but plain text instead.
	 * @param Skin $skin
	 * @return bool|void True or no return value to continue or false to abort
	 */
	public function onSkinGetPoweredBy( &$text, $skin );
}
