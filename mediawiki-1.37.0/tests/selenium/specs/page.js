'use strict';

const assert = require( 'assert' );
const Api = require( 'wdio-mediawiki/Api' );
const DeletePage = require( '../pageobjects/delete.page' );
const RestorePage = require( '../pageobjects/restore.page' );
const EditPage = require( '../pageobjects/edit.page' );
const HistoryPage = require( '../pageobjects/history.page' );
const UndoPage = require( '../pageobjects/undo.page' );
const ProtectPage = require( '../pageobjects/protect.page' );
const UserLoginPage = require( 'wdio-mediawiki/LoginPage' );
const Util = require( 'wdio-mediawiki/Util' );

describe( 'Page', function () {
	let content, name, bot;

	before( async () => {
		bot = await Api.bot();
	} );

	beforeEach( function () {
		browser.deleteAllCookies();
		content = Util.getTestString( 'beforeEach-content-' );
		name = Util.getTestString( 'BeforeEach-name-' );

		// Don't try to run wikitext-specific tests if the test namespace isn't wikitext by default.
		if ( Util.isTargetNotWikitext( name ) ) {
			this.skip();
		}
	} );

	it( 'should be previewable', function () {
		EditPage.preview( name, content );

		assert.strictEqual( EditPage.heading.getText(), 'Creating ' + name );
		assert.strictEqual( EditPage.displayedContent.getText(), content );
		assert( EditPage.content.isDisplayed(), 'editor is still present' );
		assert( !EditPage.conflictingContent.isDisplayed(), 'no edit conflict happened' );

		// T269566: Popup with text
		// 'Leave site? Changes that you made may not be saved. Cancel/Leave'
		// appears after the browser tries to leave the page with the preview.
		browser.reloadSession();
	} );

	it( 'should be creatable', function () {
		// create
		EditPage.edit( name, content );

		// check
		assert.strictEqual( EditPage.heading.getText(), name );
		assert.strictEqual( EditPage.displayedContent.getText(), content );
	} );

	it( 'should be re-creatable', function () {
		const initialContent = Util.getTestString( 'initialContent-' );

		// create and delete
		browser.call( async () => {
			await bot.edit( name, initialContent, 'create for delete' );
			await bot.delete( name, 'delete prior to recreate' );
		} );

		// re-create
		EditPage.edit( name, content );

		// check
		assert.strictEqual( EditPage.heading.getText(), name );
		assert.strictEqual( EditPage.displayedContent.getText(), content );
	} );

	it( 'should be editable @daily', function () {
		// create
		browser.call( async () => {
			await bot.edit( name, content, 'create for edit' );
		} );

		// edit
		const editContent = Util.getTestString( 'editContent-' );
		EditPage.edit( name, editContent );

		// check
		assert.strictEqual( EditPage.heading.getText(), name );
		assert( EditPage.displayedContent.getText().includes( editContent ) );
	} );

	it( 'should have history @daily', function () {
		// create
		browser.call( async () => {
			await bot.edit( name, content, `created with "${content}"` );
		} );

		// check
		HistoryPage.open( name );
		assert.strictEqual( HistoryPage.comment.getText(), `created with "${content}"` );
	} );

	it( 'should be deletable', function () {
		// create
		browser.call( async () => {
			await bot.edit( name, content, 'create for delete' );
		} );

		// login
		UserLoginPage.loginAdmin();

		// delete
		DeletePage.delete( name, 'delete reason' );

		// check
		assert.strictEqual(
			DeletePage.displayedContent.getText(),
			'"' + name + '" has been deleted. See deletion log for a record of recent deletions.\n\nReturn to Main Page.'
		);
	} );

	it( 'should be restorable', function () {
		// create and delete
		browser.call( async () => {
			await bot.edit( name, content, 'create for delete' );
			await bot.delete( name, 'delete for restore' );
		} );

		// login
		UserLoginPage.loginAdmin();

		// restore
		RestorePage.restore( name, 'restore reason' );

		// check
		assert.strictEqual( RestorePage.displayedContent.getText(), name + ' has been restored\n\nConsult the deletion log for a record of recent deletions and restorations.' );
	} );

	it( 'should be protectable', function () {
		browser.call( async () => {
			await bot.edit( name, content, 'create for protect' );
		} );

		// login
		UserLoginPage.loginAdmin();

		ProtectPage.protect(
			name,
			'protect reason',
			'Allow only administrators'
		);

		// Logout
		browser.deleteAllCookies();

		// Check that we can't edit the page anymore
		EditPage.openForEditing( name );
		assert.strictEqual( EditPage.save.isExisting(), false );
		assert.strictEqual( EditPage.heading.getText(), 'View source for ' + name );
	} );

	it.skip( 'should be undoable', function () {
		let previousRev, undoRev;
		browser.call( async () => {
			// create
			await bot.edit( name, content, 'create to edit and undo' );

			// edit
			const response = await bot.edit( name, Util.getTestString( 'editContent-' ) );
			previousRev = response.edit.oldrevid;
			undoRev = response.edit.newrevid;
		} );

		UndoPage.undo( name, previousRev, undoRev );

		assert.strictEqual( EditPage.displayedContent.getText(), content );
	} );

} );
