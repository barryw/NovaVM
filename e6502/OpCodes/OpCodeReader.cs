using System.Collections;

namespace KDS.e6502.OpCodes
{
    internal class OpCodeReader : IEnumerable<string>
    {
        private readonly List<string> _oplist;

        public OpCodeReader()
        {
            var orglist = Properties.Resources.OpCodeList.Split(new[] { Environment.NewLine }, StringSplitOptions.None);
            _oplist = new List<string>();

            // Remove the first two entries as well as blank and null lines
            for (var ii = 0; ii <= orglist.GetUpperBound(0); ii++)
            {
                if ((ii > 1) && orglist[ii].Length > 0)
                    _oplist.Add(orglist[ii]);
            }
        }

        public IEnumerator<string> GetEnumerator()
        {
            return ((IEnumerable<string>)_oplist).GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return ((IEnumerable<string>)_oplist).GetEnumerator();
        }
    }
}
