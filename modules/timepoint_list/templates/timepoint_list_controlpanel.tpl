
{if $isDataEntryPerson}
<button class="btn btn-primary" onclick="location.href='{$baseurl}/create_timepoint/?candID={$candID}&identifier={$candID}'">Create time point</button>
{/if}
{if $isDataEntryPerson}
<button class="btn btn-primary" onclick="location.href='{$baseurl}/candidate_parameters/?candID={$candID}&identifier={$candID}'">Edit Candidate Info</button>
{/if}
{if $isDataEntryPerson}
<button class="btn btn-primary" onclick="location.href='main.php?test_name=participant_status&candID={$candID}&identifier={$candID}'">Participant Status</button>
{else}
Edit Participant Status
{/if}
{if $isDataEntryPerson}
<button class="btn btn-primary" onclick="location.href='main.php?test_name=family_information&candID={$candID}&identifier={$candID}'">Family Information</button>
{else}
Edit Family Information
{/if}
{if $isDataEntryPerson}
<button class="btn btn-primary" onclick="location.href='main.php?test_name=drug_compliance&candID={$candID}&identifier={$candID}'">Drug Compliance</button>
{else}
Edit Drug Compliance
{/if}
