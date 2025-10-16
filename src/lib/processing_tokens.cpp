#include "PP_SV.h"
#include <queue>

using namespace slang;
using namespace std;
using namespace slang::syntax;
using namespace slang::parsing;


static std::unordered_map<TokenKind, format_rule> rule_table = {
    {TokenKind::Unknown, {false, true, false, false, false, false}},
    {TokenKind::EndOfFile, {false, false, false, false, false, false}},
    {TokenKind::Identifier, {false, false, true, true, false, false}},
    {TokenKind::SystemIdentifier, {false, false, false, false, false, false}},
    {TokenKind::StringLiteral, {false, false, false, false, false, false}},
    {TokenKind::IntegerLiteral, {false, false, false, false, false, false}},
    {TokenKind::IntegerBase, {false, false, false, false, false, false}},
    {TokenKind::UnbasedUnsizedLiteral, {false, false, false, false, false, false}},
    {TokenKind::RealLiteral, {false, false, false, false, false, false}},
    {TokenKind::TimeLiteral, {false, false, false, false, false, false}},
    {TokenKind::Placeholder, {false, false, false, false, false, false}},
    {TokenKind::Apostrophe, {false, false, false, false, false, false}},
    {TokenKind::ApostropheOpenBrace, {false, false, false, false, false, false}},
    {TokenKind::OpenBrace, {false, false, true, false, false, false}},
    {TokenKind::CloseBrace, {true, true, false, false, false, false}},
    {TokenKind::OpenBracket, {false, false, false, false, false, false}},
    {TokenKind::CloseBracket, {false, false, false, false, false, false}},
    {TokenKind::OpenParenthesis, {false, false, true, false, false, false}},
    {TokenKind::CloseParenthesis, {false, false, false, false, false, false}},
    {TokenKind::Semicolon, {false, true, false, false, false, false}},
    {TokenKind::Colon, {false, false, false, true, false, false}},
    {TokenKind::ColonEquals, {false, false, true, true, false, false}},
    {TokenKind::ColonSlash, {false, false, false, false, false, false}},
    {TokenKind::DoubleColon, {false, false, false, false, false, false}},
    {TokenKind::Comma, {false, true, false, true, false, false}},
    {TokenKind::Dot, {false, false, false, false, false, false}},
    {TokenKind::Slash, {false, false, true, true, false, false}},
    {TokenKind::Star, {false, false, false, false, false, false}},
    {TokenKind::DoubleStar, {false, false, true, true, false, false}},
    {TokenKind::StarArrow, {false, false, true, true, false, false}},
    {TokenKind::Plus, {false, false, true, true, false, false}},
    {TokenKind::DoublePlus, {false, false, true, true, false, false}},
    {TokenKind::PlusColon, {false, false, true, true, false, false}},
    {TokenKind::PlusDivMinus, {false, false, true, true, false, false}},
    {TokenKind::PlusModMinus, {false, false, true, true, false, false}},
    {TokenKind::Minus, {false, false, true, true, false, false}},
    {TokenKind::DoubleMinus, {false, false, true, true, false, false}},
    {TokenKind::MinusColon, {false, false, true, true, false, false}},
    {TokenKind::MinusArrow, {false, false, true, true, false, false}},
    {TokenKind::MinusDoubleArrow, {false, false, true, true, false, false}},
    {TokenKind::Tilde, {false, false, false, false, false, false}},
    {TokenKind::TildeAnd, {false, false, true, true, false, false}},
    {TokenKind::TildeOr, {false, false, true, true, false, false}},
    {TokenKind::TildeXor, {false, false, true, true, false, false}},
    {TokenKind::Dollar, {false, false, false, false, false, false}},
    {TokenKind::Question, {false, false, true, true, false, false}},
    {TokenKind::Hash, {false, false, true, true, false, false}},
    {TokenKind::DoubleHash, {false, false, true, true, false, false}},
    {TokenKind::HashMinusHash, {false, false, true, true, false, false}},
    {TokenKind::HashEqualsHash, {false, false, true, true, false, false}},
    {TokenKind::Xor, {false, false, true, true, false, false}},
    {TokenKind::XorTilde, {false, false, true, true, false, false}},
    {TokenKind::Equals, {false, false, true, true, false, false}},
    {TokenKind::DoubleEquals, {false, false, true, true, false, false}},
    {TokenKind::DoubleEqualsQuestion, {false, false, true, true, false, false}},
    {TokenKind::TripleEquals, {false, false, true, true, false, false}},
    {TokenKind::EqualsArrow, {false, false, true, true, false, false}},
    {TokenKind::PlusEqual, {false, false, true, true, false, false}},
    {TokenKind::MinusEqual, {false, false, true, true, false, false}},
    {TokenKind::SlashEqual, {false, false, true, true, false, false}},
    {TokenKind::StarEqual, {false, false, true, true, false, false}},
    {TokenKind::AndEqual, {false, false, true, true, false, false}},
    {TokenKind::OrEqual, {false, false, true, true, false, false}},
    {TokenKind::PercentEqual, {false, false, true, true, false, false}},
    {TokenKind::XorEqual, {false, false, true, true, false, false}},
    {TokenKind::LeftShiftEqual, {false, false, true, true, false, false}},
    {TokenKind::TripleLeftShiftEqual, {false, false, true, true, false, false}},
    {TokenKind::RightShiftEqual, {false, false, true, true, false, false}},
    {TokenKind::TripleRightShiftEqual, {false, false, true, true, false, false}},
    {TokenKind::LeftShift, {false, false, true, true, false, false}},
    {TokenKind::RightShift, {false, false, true, true, false, false}},
    {TokenKind::TripleLeftShift, {false, false, true, true, false, false}},
    {TokenKind::TripleRightShift, {false, false, true, true, false, false}},
    {TokenKind::Exclamation, {false, false, true, true, false, false}},
    {TokenKind::ExclamationEquals, {false, false, true, true, false, false}},
    {TokenKind::ExclamationEqualsQuestion, {false, false, true, true, false, false}},
    {TokenKind::ExclamationDoubleEquals, {false, false, true, true, false, false}},
    {TokenKind::Percent, {false, false, true, true, false, false}},
    {TokenKind::LessThan, {false, false, true, true, false, false}},
    {TokenKind::LessThanEquals, {false, false, true, true, false, false}},
    {TokenKind::LessThanMinusArrow, {false, false, true, true, false, false}},
    {TokenKind::GreaterThan, {false, false, true, true, false, false}},
    {TokenKind::GreaterThanEquals, {false, false, true, true, false, false}},
    {TokenKind::Or, {false, false, true, true, false, false}},
    {TokenKind::DoubleOr, {false, false, true, true, false, false}},
    {TokenKind::OrMinusArrow, {false, false, true, true, false, false}},
    {TokenKind::OrEqualsArrow, {false, false, true, true, false, false}},
    {TokenKind::At, {false, false, false, false, false, false}},
    {TokenKind::DoubleAt, {false, false, true, true, false, false}},
    {TokenKind::And, {false, false, true, true, false, false}},
    {TokenKind::DoubleAnd, {false, false, true, true, false, false}},
    {TokenKind::TripleAnd, {false, false, true, true, false, false}},
    {TokenKind::OneStep, {false, false, false, false, false, false}},
    
    // Keywords
    {TokenKind::AcceptOnKeyword, {false, false, true, true, false, false}},
    {TokenKind::AliasKeyword, {false, false, true, true, false, false}},
    {TokenKind::AlwaysKeyword, {true, false, true, true, false, false}},
    {TokenKind::AlwaysCombKeyword, {true, false, true, true, false, false}},
    {TokenKind::AlwaysFFKeyword, {true, false, true, true, false, false}},
    {TokenKind::AlwaysLatchKeyword, {true, false, true, true, false, false}},
    {TokenKind::AndKeyword, {false, false, true, true, false, false}},
    {TokenKind::AssertKeyword, {false, false, true, true, false, false}},
    {TokenKind::AssignKeyword, {false, false, true, true, false, false}},
    {TokenKind::AssumeKeyword, {false, false, true, true, false, false}},
    {TokenKind::AutomaticKeyword, {false, false, true, true, false, false}},
    {TokenKind::BeforeKeyword, {false, false, true, true, false, false}},
    {TokenKind::BeginKeyword, {false, true, true, false, true, false}},
    {TokenKind::BindKeyword, {false, false, true, true, false, false}},
    {TokenKind::BinsKeyword, {false, false, true, true, false, false}},
    {TokenKind::BinsOfKeyword, {false, false, true, true, false, false}},
    {TokenKind::BitKeyword, {false, false, true, true, false, false}},
    {TokenKind::BreakKeyword, {false, false, true, true, false, false}},
    {TokenKind::BufKeyword, {false, false, true, true, false, false}},
    {TokenKind::BufIf0Keyword, {false, false, true, true, false, false}},
    {TokenKind::BufIf1Keyword, {false, false, true, true, false, false}},
    {TokenKind::ByteKeyword, {false, false, true, true, false, false}},
    {TokenKind::CaseKeyword, {true, false, true, true, true, false}},
    {TokenKind::CaseXKeyword, {true, false, true, true, true, false}},
    {TokenKind::CaseZKeyword, {true, false, true, true, true, false}},
    {TokenKind::CellKeyword, {false, false, true, true, false, false}},
    {TokenKind::CHandleKeyword, {false, false, true, true, false, false}},
    {TokenKind::CheckerKeyword, {true, false, true, true, true, false}},
    {TokenKind::ClassKeyword, {true, false, true, true, true, false}},
    {TokenKind::ClockingKeyword, {false, false, true, true, true, false}},
    {TokenKind::CmosKeyword, {false, false, true, true, false, false}},
    {TokenKind::ConfigKeyword, {true, false, true, true, true, false}},
    {TokenKind::ConstKeyword, {false, false, true, true, false, false}},
    {TokenKind::ConstraintKeyword, {false, false, true, true, false, false}},
    {TokenKind::ContextKeyword, {false, false, true, true, false, false}},
    {TokenKind::ContinueKeyword, {false, false, true, true, false, false}},
    {TokenKind::CoverKeyword, {false, false, true, true, false, false}},
    {TokenKind::CoverGroupKeyword, {true, false, true, true, true, false}},
    {TokenKind::CoverPointKeyword, {false, false, true, true, false, false}},
    {TokenKind::CrossKeyword, {false, false, true, true, false, false}},
    {TokenKind::DeassignKeyword, {false, false, true, true, false, false}},
    {TokenKind::DefaultKeyword, {false, false, true, true, false, false}},
    {TokenKind::DefParamKeyword, {false, false, true, true, false, false}},
    {TokenKind::DesignKeyword, {false, false, true, true, false, false}},
    {TokenKind::DisableKeyword, {false, false, true, true, false, false}},
    {TokenKind::DistKeyword, {false, false, true, true, false, false}},
    {TokenKind::DoKeyword, {false, false, true, true, false, false}},
    {TokenKind::EdgeKeyword, {false, false, true, true, false, false}},
    {TokenKind::ElseKeyword, {true, false, true, true, false, false}},
    {TokenKind::EndKeyword, {true, true, true, false, false, true}},
    {TokenKind::EndCaseKeyword, {true, true, true, false, false, true}},
    {TokenKind::EndCheckerKeyword, {true, true, true, false, false, true}},
    {TokenKind::EndClassKeyword, {true, true, true, false, false, true}},
    {TokenKind::EndClockingKeyword, {true, true, true, false, false, true}},
    {TokenKind::EndConfigKeyword, {true, true, true, false, false, true}},
    {TokenKind::EndFunctionKeyword, {true, true, true, false, false, true}},
    {TokenKind::EndGenerateKeyword, {true, true, true, false, false, true}},
    {TokenKind::EndGroupKeyword, {true, true, true, false, false, true}},
    {TokenKind::EndInterfaceKeyword, {true, true, true, false, false, true}},
    {TokenKind::EndModuleKeyword, {true, true, true, false, false, true}},
    {TokenKind::EndPackageKeyword, {true, true, true, false, false, true}},
    {TokenKind::EndPrimitiveKeyword, {true, true, true, false, false, true}},
    {TokenKind::EndProgramKeyword, {true, true, true, false, false, true}},
    {TokenKind::EndPropertyKeyword, {true, true, true, false, false, true}},
    {TokenKind::EndSpecifyKeyword, {true, true, true, false, false, true}},
    {TokenKind::EndSequenceKeyword, {true, true, true, false, false, true}},
    {TokenKind::EndTableKeyword, {true, true, true, false, false, true}},
    {TokenKind::EndTaskKeyword, {true, true, true, false, false, true}},
    {TokenKind::EnumKeyword, {false, false, true, true, false, false}},
    {TokenKind::EventKeyword, {false, false, true, true, false, false}},
    {TokenKind::EventuallyKeyword, {false, false, true, true, false, false}},
    {TokenKind::ExpectKeyword, {false, false, true, true, false, false}},
    {TokenKind::ExportKeyword, {false, false, true, true, false, false}},
    {TokenKind::ExtendsKeyword, {false, false, true, true, false, false}},
    {TokenKind::ExternKeyword, {false, false, true, true, false, false}},
    {TokenKind::FinalKeyword, {false, false, true, true, false, false}},
    {TokenKind::FirstMatchKeyword, {false, false, true, true, false, false}},
    {TokenKind::ForKeyword, {false, false, true, true, false, false}},
    {TokenKind::ForceKeyword, {false, false, true, true, false, false}},
    {TokenKind::ForeachKeyword, {false, false, true, true, false, false}},
    {TokenKind::ForeverKeyword, {false, false, true, true, false, false}},
    {TokenKind::ForkKeyword, {false, false, true, true, true, false}},
    {TokenKind::ForkJoinKeyword, {false, false, true, true, false, true}},
    {TokenKind::FunctionKeyword, {true, false, true, true, true, false}},
    {TokenKind::GenerateKeyword, {true, false, true, true, true, false}},
    {TokenKind::GenVarKeyword, {false, false, true, true, false, false}},
    {TokenKind::GlobalKeyword, {false, false, true, true, false, false}},
    {TokenKind::HighZ0Keyword, {false, false, true, true, false, false}},
    {TokenKind::HighZ1Keyword, {false, false, true, true, false, false}},
    {TokenKind::IfKeyword, {false, false, true, true, false, false}},
    {TokenKind::IffKeyword, {false, false, true, true, false, false}},
    {TokenKind::IfNoneKeyword, {false, false, true, true, false, false}},
    {TokenKind::IgnoreBinsKeyword, {false, false, true, true, false, false}},
    {TokenKind::IllegalBinsKeyword, {false, false, true, true, false, false}},
    {TokenKind::ImplementsKeyword, {false, false, true, true, false, false}},
    {TokenKind::ImpliesKeyword, {false, false, true, true, false, false}},
    {TokenKind::ImportKeyword, {false, false, true, true, false, false}},
    {TokenKind::IncDirKeyword, {false, false, true, true, false, false}},
    {TokenKind::IncludeKeyword, {false, false, true, true, false, false}},
    {TokenKind::InitialKeyword, {true, false, true, true, false, false}},
    {TokenKind::InOutKeyword, {false, false, true, true, false, false}},
    {TokenKind::InputKeyword, {false, false, true, true, false, false}},
    {TokenKind::InsideKeyword, {false, false, true, true, false, false}},
    {TokenKind::InstanceKeyword, {false, false, true, true, false, false}},
    {TokenKind::IntKeyword, {false, false, true, true, false, false}},
    {TokenKind::IntegerKeyword, {false, false, true, true, false, false}},
    {TokenKind::InterconnectKeyword, {false, false, true, true, false, false}},
    {TokenKind::InterfaceKeyword, {true, false, true, true, true, false}},
    {TokenKind::IntersectKeyword, {false, false, true, true, false, false}},
    {TokenKind::JoinKeyword, {true, true, true, false, false, true}},
    {TokenKind::JoinAnyKeyword, {true, true, true, false, false, true}},
    {TokenKind::JoinNoneKeyword, {true, true, true, false, false, true}},
    {TokenKind::LargeKeyword, {false, false, true, true, false, false}},
    {TokenKind::LetKeyword, {false, false, true, true, false, false}},
    {TokenKind::LibListKeyword, {false, false, true, true, false, false}},
    {TokenKind::LibraryKeyword, {false, false, true, true, false, false}},
    {TokenKind::LocalKeyword, {false, false, true, true, false, false}},
    {TokenKind::LocalParamKeyword, {false, false, true, true, false, false}},
    {TokenKind::LogicKeyword, {false, false, true, true, false, false}},
    {TokenKind::LongIntKeyword, {false, false, true, true, false, false}},
    {TokenKind::MacromoduleKeyword, {true, false, true, true, true, false}},
    {TokenKind::MatchesKeyword, {false, false, true, true, false, false}},
    {TokenKind::MediumKeyword, {false, false, true, true, false, false}},
    {TokenKind::ModPortKeyword, {false, false, true, true, false, false}},
    {TokenKind::ModuleKeyword, {true, false, true, true, true, false}},
    {TokenKind::NandKeyword, {false, false, true, true, false, false}},
    {TokenKind::NegEdgeKeyword, {false, false, true, true, false, false}},
    {TokenKind::NetTypeKeyword, {false, false, true, true, false, false}},
    {TokenKind::NewKeyword, {false, false, true, true, false, false}},
    {TokenKind::NextTimeKeyword, {false, false, true, true, false, false}},
    {TokenKind::NmosKeyword, {false, false, true, true, false, false}},
    {TokenKind::NorKeyword, {false, false, true, true, false, false}},
    {TokenKind::NoShowCancelledKeyword, {false, false, true, true, false, false}},
    {TokenKind::NotKeyword, {false, false, true, true, false, false}},
    {TokenKind::NotIf0Keyword, {false, false, true, true, false, false}},
    {TokenKind::NotIf1Keyword, {false, false, true, true, false, false}},
    {TokenKind::NullKeyword, {false, false, true, true, false, false}},
    {TokenKind::OrKeyword, {false, false, true, true, false, false}},
    {TokenKind::OutputKeyword, {false, false, true, true, false, false}},
    {TokenKind::PackageKeyword, {true, false, true, true, true, false}},
    {TokenKind::PackedKeyword, {false, false, true, true, false, false}},
    {TokenKind::ParameterKeyword, {false, false, true, true, false, false}},
    {TokenKind::PmosKeyword, {false, false, true, true, false, false}},
    {TokenKind::PosEdgeKeyword, {false, false, true, true, false, false}},
    {TokenKind::PrimitiveKeyword, {true, false, true, true, true, false}},
    {TokenKind::PriorityKeyword, {false, false, true, true, false, false}},
    {TokenKind::ProgramKeyword, {true, false, true, true, true, false}},
    {TokenKind::PropertyKeyword, {true, false, true, true, true, false}},
    {TokenKind::ProtectedKeyword, {false, false, true, true, false, false}},
    {TokenKind::Pull0Keyword, {false, false, true, true, false, false}},
    {TokenKind::Pull1Keyword, {false, false, true, true, false, false}},
    {TokenKind::PullDownKeyword, {false, false, true, true, false, false}},
    {TokenKind::PullUpKeyword, {false, false, true, true, false, false}},
    {TokenKind::PulseStyleOnDetectKeyword, {false, false, true, true, false, false}},
    {TokenKind::PulseStyleOnEventKeyword, {false, false, true, true, false, false}},
    {TokenKind::PureKeyword, {false, false, true, true, false, false}},
    {TokenKind::RandKeyword, {false, false, true, true, false, false}},
    {TokenKind::RandCKeyword, {false, false, true, true, false, false}},
    {TokenKind::RandCaseKeyword, {false, false, true, true, true, false}},
    {TokenKind::RandSequenceKeyword, {false, false, true, true, true, false}},
    {TokenKind::RcmosKeyword, {false, false, true, true, false, false}},
    {TokenKind::RealKeyword, {false, false, true, true, false, false}},
    {TokenKind::RealTimeKeyword, {false, false, true, true, false, false}},
    {TokenKind::RefKeyword, {false, false, true, true, false, false}},
    {TokenKind::RegKeyword, {false, false, true, true, false, false}},
    {TokenKind::RejectOnKeyword, {false, false, true, true, false, false}},
    {TokenKind::ReleaseKeyword, {false, false, true, true, false, false}},
    {TokenKind::RepeatKeyword, {false, false, true, true, false, false}},
    {TokenKind::RestrictKeyword, {false, false, true, true, false, false}},
    {TokenKind::ReturnKeyword, {false, false, true, true, false, false}},
    {TokenKind::RnmosKeyword, {false, false, true, true, false, false}},
    {TokenKind::RpmosKeyword, {false, false, true, true, false, false}},
    {TokenKind::RtranKeyword, {false, false, true, true, false, false}},
    {TokenKind::RtranIf0Keyword, {false, false, true, true, false, false}},
    {TokenKind::RtranIf1Keyword, {false, false, true, true, false, false}},
    {TokenKind::SAlwaysKeyword, {false, false, true, true, false, false}},
    {TokenKind::SEventuallyKeyword, {false, false, true, true, false, false}},
    {TokenKind::SNextTimeKeyword, {false, false, true, true, false, false}},
    {TokenKind::SUntilKeyword, {false, false, true, true, false, false}},
    {TokenKind::SUntilWithKeyword, {false, false, true, true, false, false}},
    {TokenKind::ScalaredKeyword, {false, false, true, true, false, false}},
    {TokenKind::SequenceKeyword, {true, false, true, true, true, false}},
    {TokenKind::ShortIntKeyword, {false, false, true, true, false, false}},
    {TokenKind::ShortRealKeyword, {false, false, true, true, false, false}},
    {TokenKind::ShowCancelledKeyword, {false, false, true, true, false, false}},
    {TokenKind::SignedKeyword, {false, false, true, true, false, false}},
    {TokenKind::SmallKeyword, {false, false, true, true, false, false}},
    {TokenKind::SoftKeyword, {false, false, true, true, false, false}},
    {TokenKind::SolveKeyword, {false, false, true, true, false, false}},
    {TokenKind::SpecifyKeyword, {true, false, true, true, true, false}},
    {TokenKind::SpecParamKeyword, {false, false, true, true, false, false}},
    {TokenKind::StaticKeyword, {false, false, true, true, false, false}},
    {TokenKind::StringKeyword, {false, false, true, true, false, false}},
    {TokenKind::StrongKeyword, {false, false, true, true, false, false}},
    {TokenKind::Strong0Keyword, {false, false, true, true, false, false}},
    {TokenKind::Strong1Keyword, {false, false, true, true, false, false}},
    {TokenKind::StructKeyword, {false, false, true, true, false, false}},
    {TokenKind::SuperKeyword, {false, false, true, true, false, false}},
    {TokenKind::Supply0Keyword, {false, false, true, true, false, false}},
    {TokenKind::Supply1Keyword, {false, false, true, true, false, false}},
    {TokenKind::SyncAcceptOnKeyword, {false, false, true, true, false, false}},
    {TokenKind::SyncRejectOnKeyword, {false, false, true, true, false, false}},
    {TokenKind::TableKeyword, {true, false, true, true, true, false}},
    {TokenKind::TaggedKeyword, {false, false, true, true, false, false}},
    {TokenKind::TaskKeyword, {true, false, true, true, true, false}},
    {TokenKind::ThisKeyword, {false, false, true, true, false, false}},
    {TokenKind::ThroughoutKeyword, {false, false, true, true, false, false}},
    {TokenKind::TimeKeyword, {false, false, true, true, false, false}},
    {TokenKind::TimePrecisionKeyword, {false, false, true, true, false, false}},
    {TokenKind::TimeUnitKeyword, {false, false, true, true, false, false}},
    {TokenKind::TranKeyword, {false, false, true, true, false, false}},
    {TokenKind::TranIf0Keyword, {false, false, true, true, false, false}},
    {TokenKind::TranIf1Keyword, {false, false, true, true, false, false}},
    {TokenKind::TriKeyword, {false, false, true, true, false, false}},
    {TokenKind::Tri0Keyword, {false, false, true, true, false, false}},
    {TokenKind::Tri1Keyword, {false, false, true, true, false, false}},
    {TokenKind::TriAndKeyword, {false, false, true, true, false, false}},
    {TokenKind::TriOrKeyword, {false, false, true, true, false, false}},
    {TokenKind::TriRegKeyword, {false, false, true, true, false, false}},
    {TokenKind::TypeKeyword, {false, false, true, true, false, false}},
    {TokenKind::TypedefKeyword, {false, false, true, true, false, false}},
    {TokenKind::UnionKeyword, {false, false, true, true, false, false}},
    {TokenKind::UniqueKeyword, {false, false, true, true, false, false}},
    {TokenKind::Unique0Keyword, {false, false, true, true, false, false}},
    {TokenKind::UnsignedKeyword, {false, false, true, true, false, false}},
    {TokenKind::UntilKeyword, {false, false, true, true, false, false}},
    {TokenKind::UntilWithKeyword, {false, false, true, true, false, false}},
    {TokenKind::UntypedKeyword, {false, false, true, true, false, false}},
    {TokenKind::UseKeyword, {false, false, true, true, false, false}},
    {TokenKind::UWireKeyword, {false, false, true, true, false, false}},
    {TokenKind::VarKeyword, {false, false, true, true, false, false}},
    {TokenKind::VectoredKeyword, {false, false, true, true, false, false}},
    {TokenKind::VirtualKeyword, {false, false, true, true, false, false}},
    {TokenKind::VoidKeyword, {false, false, true, true, false, false}},
    {TokenKind::WaitKeyword, {false, false, true, true, false, false}},
    {TokenKind::WaitOrderKeyword, {false, false, true, true, false, false}},
    {TokenKind::WAndKeyword, {false, false, true, true, false, false}},
    {TokenKind::WeakKeyword, {false, false, true, true, false, false}},
    {TokenKind::Weak0Keyword, {false, false, true, true, false, false}},
    {TokenKind::Weak1Keyword, {false, false, true, true, false, false}},
    {TokenKind::WhileKeyword, {false, false, true, true, false, false}},
    {TokenKind::WildcardKeyword, {false, false, true, true, false, false}},
    {TokenKind::WireKeyword, {false, false, true, true, false, false}},
    {TokenKind::WithKeyword, {false, false, true, true, false, false}},
    {TokenKind::WithinKeyword, {false, false, true, true, false, false}},
    {TokenKind::WOrKeyword, {false, false, true, true, false, false}},
    {TokenKind::XnorKeyword, {false, false, true, true, false, false}},
    {TokenKind::XorKeyword, {false, false, true, true, false, false}},
    
    {TokenKind::UnitSystemName, {false, false, false, false, false, false}},
    {TokenKind::RootSystemName, {false, false, false, false, false, false}},
    {TokenKind::Directive, {false, false, false, false, false, false}},
    {TokenKind::IncludeFileName, {false, false, false, false, false, false}},
    {TokenKind::MacroUsage, {false, false, false, false, false, false}},
    {TokenKind::MacroQuote, {false, false, false, false, false, false}},
    {TokenKind::MacroTripleQuote, {false, false, false, false, false, false}},
    {TokenKind::MacroEscapedQuote, {false, false, false, false, false, false}},
    {TokenKind::MacroPaste, {false, false, false, false, false, false}},
    {TokenKind::EmptyMacroArgument, {false, false, false, false, false, false}},
    {TokenKind::LineContinuation, {false, false, false, false, false, false}}
};
    


int format_tokens(vector<my_token>& tokens){
    vector<layout_item> layout;
    int currentIndent = 0;

    bool next_blockStart_valid = true;

    bool disable_or_wait_flag = false;

    for (size_t i = 0; i < tokens.size(); i++) {
        const my_token& tok = tokens[i];

        auto rule = rule_table.find(tok.kind);

        format_rule rule_cur = (rule != rule_table.end())
            ? rule->second
            : format_rule{false,false,true,true,false,false};

        layout_item item;

        if(rule_cur.blockEnd){
            //cout << "!!!"<<tok.text;
             currentIndent--;
        }

        item.text = tok.text;
        item.indentLevel = currentIndent;

        if(rule_cur.blockStart){
            //cout << "!!!"<<tok.text;
             currentIndent++;
        }
        if(rule_cur.spaceAfter) item.spaceAfter = true;
        if(rule_cur.spaceBefore) item.spaceBefore = true;
        if(rule_cur.newlineAfter) item.newlineAfter = true;
        if(rule_cur.newlineBefore) item.newlineBefore = true;

        
        //Особые случаи
        if(disable_or_wait_flag){
            if(tok.kind == TokenKind::ForkKeyword
            || tok.kind == TokenKind::TaskKeyword
            || tok.kind == TokenKind::FunctionKeyword){
                item.newlineBefore = false;
                currentIndent--;
            }
            disable_or_wait_flag = false;
        }

        if(tok.kind == TokenKind::DisableKeyword || tok.kind == TokenKind::WaitKeyword) disable_or_wait_flag = true;

        if(tok.kind == TokenKind::InterfaceKeyword && tokens[i + 1].kind == TokenKind::ClassKeyword){
                currentIndent--;
        }
        if(i > 0 && tok.kind == TokenKind::ClassKeyword 
             && tokens[i - 1].kind == TokenKind::VirtualKeyword){
                item.newlineBefore = false;
        }
        if(i > 0 && tok.kind == TokenKind::InterfaceKeyword 
             && tokens[i - 1].kind == TokenKind::VirtualKeyword){
                item.newlineBefore = false;
                currentIndent--;
        }
        if(i > 0 && tokens[i - 1].kind == TokenKind::InterfaceKeyword){
            if(tok.kind == TokenKind::ClassKeyword){
                item.newlineBefore = false;
            }
        }
        if(i > 0 && tokens[i - 1].kind == TokenKind::TypedefKeyword){
            if(tok.kind == TokenKind::ClassKeyword || tok.kind == TokenKind::InterfaceKeyword){
                item.newlineBefore = false;
                currentIndent--;
            }
        }

        if(tok.kind == TokenKind::ExternKeyword || tok.kind == TokenKind::PureKeyword){
            next_blockStart_valid = false;
        }
        if(rule_cur.blockStart && !next_blockStart_valid){
            if(tok.kind == TokenKind::FunctionKeyword
            || tok.kind == TokenKind::TaskKeyword
            || tok.kind == TokenKind::ConstraintKeyword){
                item.newlineBefore = false;
                currentIndent--;
            }
            next_blockStart_valid = true;
        }
        if(i > 0 && tok.kind == TokenKind::JoinKeyword){
            if(tokens[i - 1].kind == TokenKind::RandKeyword){
                item.newlineBefore = false;
                currentIndent++;
            }
        }
        if(tok.kind == TokenKind::PropertyKeyword && tokens[i + 1].kind != TokenKind::Identifier){
            currentIndent--;
        }

        //Конец особых случаев

        layout.push_back(item);
    }

    for (size_t i = 0; i < layout.size(); i++) {
        auto item = layout[i];
        if(item.newlineBefore){ 
            cout << '\n';
            string s(item.indentLevel,'\t');
            cout << s;
        }
        if(item.spaceBefore) cout << ' ';
        cout << item.text;
        if(item.spaceAfter) cout << ' ';
        if(item.newlineAfter){ 
            cout << '\n';
            string s(item.indentLevel,'\t');
            cout << s;
        }
    }

    return currentIndent;
}
