#
# All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
# its licensors.
#
# For complete copyright and license terms please see the LICENSE at the root of this
# distribution (the "License"). All use of this software is governed by the License,
# or, if provided, by the license below or the license accompanying this file. Do not
# remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#

# lmbrwaflib imports
from lmbrwaflib.cry_utils import append_to_unique_list


def load_test_settings(ctx):
    """
    Setup all compiler and linker settings shared over all dedicated configurations
    """
    # Setup defines
    append_to_unique_list(
        ctx.env['DEFINES'], [
            'AZ_TESTS_ENABLED',
            'AZCORE_ENABLE_MEMORY_TRACKING'         # Enable memory tracking for all unit tests
            ])
